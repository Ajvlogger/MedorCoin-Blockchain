#include "blockchain.h"
#include "block.h"
#include "transaction.h"
#include "utxo.h"
#include "consensus.h"
#include <ctime>
#include <cstdint>
#include <vector>
#include <string>
#include <iostream>

// -------------------------------
// Constructor
// -------------------------------
Blockchain::Blockchain(const std::string& ownerAddr) {
    ownerAddress = ownerAddr;
    medor = 0x1e00ffff;       // Initial difficulty
    totalSupply = 0;
    maxSupply = 50000000;     // Max supply of MedorCoin
    powEngine = ProofOfWork(4); // Default difficulty 4 leading zeros
}

// -------------------------------
// Calculate reward per block
// -------------------------------
uint64_t Blockchain::calculateReward() {
    time_t now = time(nullptr);
    time_t genesisTime = chain.empty() ? now : chain.front().timestamp;
    double months = difftime(now, genesisTime) / (60 * 60 * 24 * 30.0);

    // First 2 months: 0.02 USD/block (simulate)
    return (months <= 2.0) ? 55 : 30;
}

// -------------------------------
// Mine a block using Proof-of-Work
// -------------------------------
void Blockchain::mineBlock(Block& block) {
    powEngine.mineBlock(block);
}

// -------------------------------
// Add a block to the blockchain
// -------------------------------
void Blockchain::addBlock(const std::string& minerAddr,
                          std::vector<Transaction>& transactions) {
    // 1. Calculate block reward
    uint64_t reward = calculateReward();
    if (totalSupply + reward > maxSupply)
        reward = maxSupply - totalSupply;

    // 2. Create coinbase transaction
    Transaction coinbaseTx;
    TxOutput minerOut, ownerOut;

    minerOut.value = (reward * 90) / 100;
    minerOut.address = minerAddr;

    ownerOut.value = reward - minerOut.value;
    ownerOut.address = ownerAddress;

    coinbaseTx.outputs.push_back(minerOut);
    coinbaseTx.outputs.push_back(ownerOut);
    coinbaseTx.calculateHash();

    // 3. Insert coinbase transaction at start
    transactions.insert(transactions.begin(), coinbaseTx);

    // 4. Create new block
    Block newBlock;
    if (!chain.empty())
        newBlock = Block(chain.back().hash, "MedorCoin Block", medor, minerAddr);
    else
        newBlock = Block("", "Genesis Block", medor, minerAddr);

    newBlock.timestamp = time(nullptr);
    newBlock.reward = reward;
    newBlock.transactions = transactions;

    // 5. Mine the block using Proof-of-Work
    mineBlock(newBlock);

    // 6. Validate PoW before adding
    if (!powEngine.validateBlock(newBlock)) {
        std::cerr << "Error: Block PoW invalid!" << std::endl;
        return;
    }

    // 7. Add block to chain
    chain.push_back(newBlock);

    // 8. Update UTXO set
    for (auto& tx : newBlock.transactions) {
        for (auto& in : tx.inputs)
            utxoSet.spendUTXO(in.prevTxHash, in.outputIndex);

        for (size_t i = 0; i < tx.outputs.size(); ++i)
            utxoSet.addUTXO(tx.outputs[i], tx.txHash, static_cast<int>(i));
    }

    // 9. Update total supply
    totalSupply += reward;

    std::cout << "Block added! Hash: " << newBlock.hash
              << " | Nonce: " << newBlock.nonce
              << " | Reward: " << reward << std::endl;
}

// -------------------------------
// Optional: Print chain for debug
// -------------------------------
void Blockchain::printChain() const {
    std::cout << "------ MedorCoin Blockchain ------" << std::endl;
    for (size_t i = 0; i < chain.size(); ++i) {
        const Block& b = chain[i];
        std::cout << "Block " << i
                  << " | Hash: " << b.hash
                  << " | Prev: " << b.previousHash
                  << " | Time: " << b.timestamp
                  << " | Nonce: " << b.nonce
                  << " | Tx Count: " << b.transactions.size()
                  << std::endl;
    }
}
