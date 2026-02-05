#include "blockchain.h"
#include "block.h"
#include "transaction.h"
#include "utxo.h"
#include "consensus.h"
#include "db/blockdb.h"  // LevelDB storage
#include <ctime>
#include <cstdint>
#include <vector>
#include <string>
#include <iostream>

// -------------------------------
// Constructor
// -------------------------------
Blockchain::Blockchain(const std::string& ownerAddr)
    : blockDB() // Initialize the LevelDB object
{
    ownerAddress = ownerAddr;
    medor = 0x1e00ffff;       // Initial difficulty
    totalSupply = 0;
    maxSupply = 50000000;     // Max supply of MedorCoin
    powEngine = ProofOfWork(4); // Default difficulty 4 leading zeros

    // Open LevelDB database for storing blocks
    if (!blockDB.open("data/medorcoin_blocks")) {
        std::cerr << "Error: Could not open LevelDB storage" << std::endl;
    }

    // Load blocks from LevelDB into memory
    leveldb::Iterator* it = blockDB.db->NewIterator(leveldb::ReadOptions());
    for (it->SeekToFirst(); it->Valid(); it->Next()) {
        Block b;
        const std::string hashKey = it->key().ToString();
        if (blockDB.readBlock(hashKey, b)) {
            chain.push_back(b);
        }
    }
    delete it;

    // If the chain is empty (first run), create the genesis block
    if (chain.empty()) {
        std::cout << "No blocks found in DB — creating genesis block" << std::endl;
        std::vector<Transaction> noTxs;
        addBlock(ownerAddress, noTxs);
    }
}

// -------------------------------
// Calculate reward per block
// -------------------------------
uint64_t Blockchain::calculateReward() {
    time_t now = time(nullptr);
    time_t genesisTime = chain.empty() ? now : chain.front().timestamp;
    double months = difftime(now, genesisTime) / (60 * 60 * 24 * 30.0);

    // First 2 months: 0.02 USD/block (reward 55)
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
                          std::vector<Transaction>& transactions) 
{
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

    // Insert coinbase at start
    transactions.insert(transactions.begin(), coinbaseTx);

    // 3. Create new block
    Block newBlock;
    if (!chain.empty())
        newBlock = Block(chain.back().hash, "MedorCoin Block", medor, minerAddr);
    else
        newBlock = Block("", "Genesis Block", medor, minerAddr);

    newBlock.timestamp = time(nullptr);
    newBlock.reward = reward;
    newBlock.transactions = transactions;

    // 4. Mine the block
    mineBlock(newBlock);

    // 5. Validate PoW
    if (!powEngine.validateBlock(newBlock)) {
        std::cerr << "Error: Block PoW invalid!" << std::endl;
        return;
    }

    // 6. Add to in‑memory chain
    chain.push_back(newBlock);

    // 7. Persist to LevelDB
    if (!blockDB.writeBlock(newBlock)) {
        std::cerr << "Warning: Failed to write block to DB!" << std::endl;
    }

    // 8. Update UTXO set
    for (auto& tx : newBlock.transactions) {
        for (auto& in : tx.inputs)
            utxoSet.spendUTXO(in.prevTxHash, in.outputIndex);

        for (size_t i = 0; i < tx.outputs.size(); ++i)
            utxoSet.addUTXO(tx.outputs[i], tx.txHash, static_cast<int>(i));
    }

    // 9. Update total supply
    totalSupply += reward;

    std::cout << "Block added. Hash: " << newBlock.hash
              << " | Nonce: " << newBlock.nonce
              << " | Reward: " << reward << std::endl;
}

// -------------------------------
// Chain validation
// -------------------------------
bool Blockchain::validateBlock(const Block& block, const Block& previousBlock) {
    if (block.previousHash != previousBlock.hash) return false;
    if (block.timestamp <= previousBlock.timestamp) return false;

    for (auto& tx : block.transactions) {
        if (tx.txHash.empty()) return false;
    }
    return true;
}

bool Blockchain::validateChain() {
    for (size_t i = 1; i < chain.size(); ++i)
        if (!validateBlock(chain[i], chain[i - 1]))
            return false;
    return true;
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
                  << " | TX Count: " << b.transactions.size()
                  << std::endl;
    }
}
