// SPDX-License-Identifier: MIT
pragma solidity ^0.8.20;

import "@openzeppelin/contracts/token/ERC20/ERC20.sol";
import "@openzeppelin/contracts/utils/ReentrancyGuard.sol";
import "@openzeppelin/contracts/access/Ownable.sol";

/**
 * @dev Standard ERC20 interface to handle external WBTC tokens securely.
 */
interface IERC20 {
    function transferFrom(address sender, address recipient, uint256 amount) external returns (bool);
    function transfer(address recipient, uint256 amount) external returns (bool);
    function balanceOf(address account) external view returns (uint256);
}

contract MedorToken is ERC20, ReentrancyGuard, Ownable {
    
    // Industrial Target: Hardened to ~32 leading zero bits initially
    uint256 public miningTarget = 0x00000000ffffffffffffffffffffffffffffffffffffffffffffffffffffffff;
    uint256 public constant MINING_REWARD = 50 * 10**18; 
    
    // Difficulty bounds to ensure owner configurations cannot break the network
    uint256 public constant MAX_DIFFICULTY_TARGET = 0x0000ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff;
    uint256 public constant MIN_DIFFICULTY_TARGET = 0x000000000000000000000000000000000000000000000000000000000000ffff;

    mapping(bytes32 => bool) public usedSolutions;
    
    // Tracks the single active commitment per miner to prevent storage pollution
    mapping(address => bytes32) public activeCommitment;
    mapping(address => uint256) public commitmentBlock;

    // --- INTERNAL PRE-LAUNCH SWAP STATE ---
    address public immutable wbtcTokenAddress; 
    uint256 public tokensPerWBTC;     
    bool public isSwapActive;        

    // --- PRODUCTION EVENTS ---
    event TokenSwapped(address indexed buyer, uint256 wbtcSpent, uint256 medorReceived);
    event SwapStatusChanged(bool isActive);
    event SwapRateUpdated(uint256 oldRate, uint256 newRate);
    event DifficultyChanged(uint256 oldTarget, uint256 newTarget);
    event RewardClaimed(address indexed miner, bytes32 indexed solutionHash, uint256 amount);

    constructor(address initialReceiver, address _wbtcAddress, uint256 _rate) 
        ERC20("Medor Token", "MEDOR") 
        Ownable(initialReceiver) 
    {
        require(_wbtcAddress != address(0), "Invalid WBTC address");
        require(_rate > 0, "Initial rate must be positive");

        _mint(initialReceiver, 20_000_000 * 10**18);
        _mint(address(this), 30_000_000 * 10**18);

        wbtcTokenAddress = _wbtcAddress;
        tokensPerWBTC = _rate;
        isSwapActive = false;
    }

    // --- STEP-BY-STEP SWAP FUNCTIONS ---

    /**
     * @notice Step 1: Owner toggles the swap status to allow or block transactions.
     */
    function toggleSwap(bool _status) external onlyOwner {
        isSwapActive = _status;
        emit SwapStatusChanged(_status);
    }

    /**
     * @notice Optional: Owner updates the swap rate dynamically to adapt to market pricing.
     */
    function updateSwapRate(uint256 _newRate) external onlyOwner {
        require(_newRate > 0, "Rate must be greater than zero");
        uint256 oldRate = tokensPerWBTC;
        tokensPerWBTC = _newRate;
        emit SwapRateUpdated(oldRate, _newRate);
    }

    /**
     * @notice Step 2: Users trade their WBTC for MEDOR tokens internally.
     */
    function swapWBTCForMedor(uint256 wbtcAmount) external nonReentrant {
        require(isSwapActive, "Pre-launch swap is currently closed");
        require(wbtcAmount > 0, "Amount must be greater than zero");

        // Calculate MEDOR amount (handles WBTC's 8 decimals vs MEDOR's 18 decimals)
        uint256 medorAmount = (wbtcAmount * tokensPerWBTC * 10**18) / 10**8;
        require(balanceOf(address(this)) >= medorAmount, "Insufficient MEDOR in swap pool");

        // Transfer the WBTC into the contract safely
        bool success = IERC20(wbtcTokenAddress).transferFrom(msg.sender, address(this), wbtcAmount);
        require(success, "WBTC transfer failed");

        // Distribute the MEDOR tokens instantly
        _transfer(address(this), msg.sender, medorAmount);
        emit TokenSwapped(msg.sender, wbtcAmount, medorAmount);
    }

    /**
     * @notice Step 3: Owner withdraws the collected WBTC safely.
     */
    function withdrawCollectedWBTC(address recipient) external onlyOwner {
        require(recipient != address(0), "Invalid recipient address");
        uint256 totalWBTC = IERC20(wbtcTokenAddress).balanceOf(address(this));
        require(totalWBTC > 0, "No WBTC available to withdraw");
        
        bool success = IERC20(wbtcTokenAddress).transfer(recipient, totalWBTC);
        require(success, "Withdrawal failed");
    }

    // --- ORIGINAL MINING LOGIC ---

    /**
     * @notice Step 1: Commit solution hash. Overwrites are allowed if the previous one expired (>256 blocks).
     */
    function commitSolution(bytes32 commitmentHash) external {
        uint256 commitBlock = commitmentBlock[msg.sender];
        require(
            activeCommitment[msg.sender] == 0 || block.number > commitBlock + 256, 
            "Existing active commitment pending confirmation"
        );
        activeCommitment[msg.sender] = commitmentHash;
        commitmentBlock[msg.sender] = block.number;
    }

    /**
     * @notice Step 2: Reveal nonce and claim reward within the valid block validation window.
     */
    function claimMiningReward(uint64 nonce) external nonReentrant {
        bytes32 solutionHash = keccak256(abi.encodePacked(msg.sender, nonce));
        bytes32 expectedCommitment = keccak256(abi.encodePacked(solutionHash));
        
        uint256 commitBlock = commitmentBlock[msg.sender];
        
        // Enforce anti-front-running block boundary rules
        require(activeCommitment[msg.sender] == expectedCommitment, "Invalid or missing commitment");
        require(block.number > commitBlock, "Must wait at least 1 block confirmation");
        require(block.number <= commitBlock + 256, "Commitment expired. Must be claimed within 256 blocks");
        require(!usedSolutions[solutionHash], "Solution already exploited");
        
        // Validate solution against numeric work metric
        require(uint256(solutionHash) < miningTarget, "Hash does not meet target");

        // Clean state storage variables to reclaim gas and prevent bloat
        usedSolutions[solutionHash] = true;
        delete activeCommitment[msg.sender];
        delete commitmentBlock[msg.sender];
        
        uint256 contractBalance = balanceOf(address(this));
        require(contractBalance >= MINING_REWARD, "Mining pool exhausted");
        
        _transfer(address(this), msg.sender, MINING_REWARD);
        emit RewardClaimed(msg.sender, solutionHash, MINING_REWARD);
    }

    /**
     * @notice Secure control parameter modifier for difficulty management
     */
    function adjustDifficulty(uint256 newTarget) external onlyOwner {
        require(newTarget >= MIN_DIFFICULTY_TARGET && newTarget <= MAX_DIFFICULTY_TARGET, "Target out of safe bounds");
        uint256 oldTarget = miningTarget;
        miningTarget = newTarget;
        emit DifficultyChanged(oldTarget, newTarget);
    }
}
