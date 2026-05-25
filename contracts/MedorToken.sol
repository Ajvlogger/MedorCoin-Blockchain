// SPDX-License-Identifier: MIT
pragma solidity ^0.8.20;

import "@openzeppelin/contracts/token/ERC20/ERC20.sol";
import "@openzeppelin/contracts/utils/ReentrancyGuard.sol";
import "@openzeppelin/contracts/access/Ownable.sol";

contract MedorToken is ERC20, ReentrancyGuard, Ownable {
    
    // Industrial Target: Hardened to ~32 leading zero bits initially
    uint256 public miningTarget = 0x00000000ffffffffffffffffffffffffffffffffffffffffffffffffffffffff;
    uint256 public constant MINING_REWARD = 50 * 10**18; 
    
    mapping(bytes32 => bool) public usedSolutions;
    
    // Tracks the single active commitment per miner to prevent storage pollution
    mapping(address => bytes32) public activeCommitment;
    mapping(address => uint256) public commitmentBlock;

    constructor(address initialReceiver) 
        ERC20("Medor Token", "MEDOR") 
        Ownable(initialReceiver) 
    {
        _mint(initialReceiver, 20_000_000 * 10**18);
        _mint(address(this), 30_000_000 * 10**18);
    }

    /**
     * @notice Step 1: Commit solution hash. Overwrites any previous unrevealed commitment.
     */
    function commitSolution(bytes32 commitmentHash) external {
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
    }

    /**
     * @notice Secure control parameter modifier for difficulty management
     */
    function adjustDifficulty(uint256 newTarget) external onlyOwner {
        miningTarget = newTarget;
    }
}
