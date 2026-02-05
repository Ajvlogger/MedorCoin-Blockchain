#include "blockchain.h"
#include "block.h"
#include "transaction.h"
#include <iostream>
#include <ctime>
#include <sstream>

// Constructor
Blockchain::Blockchain(const std::string& ownerAddr) {
    ownerAddress = ownerAddr;
    medor = 0x1e00ffff;
    totalSupply = 0;
    maxSupply = 50000000;
}

// Calculate reward
uint64_t Blockchain::calculateReward() {
    time_t now = time(nullptr);
    time_t genesis = chain.empty() ? now : chain.front().timestamp;
    double months = difftime(now, genesis) / (60 * 60 * 24 * 30.0);
    return (months <= 2.0) ? 55 : 30;
}

// Lightweight mining
void Blockchain::mineBlock(Block& block) {
    std::string target = "0"; // Dummy PoW for simplicity
    do {
        block.nonce++;
        block.hash = block.headerToString(); // For real, replace with doubleSHA256
    } while (block.hash.substr(0, target.size()) != target);
}

// Add block
void Blockchain::addBlock(const std::string& minerAddress, std::vector<Transaction>& transactions) {
    std::lock_guard<std::mutex> lock(mtx);

    uint64_t reward = calculateReward();
    if (totalSupply + reward > maxSupply) reward = maxSupply - totalSupply;

    // Coinbase tx
    Transaction coinbaseTx;
    TxOutput minerOut, ownerOut;
    minerOut.value = (reward * 90) / 100; minerOut.address = minerAddress;
    ownerOut.value = reward - minerOut.value; ownerOut.address = ownerAddress;
    coinbaseTx.outputs.push_back(minerOut);
    coinbaseTx.outputs.push_back(ownerOut);
    coinbaseTx.calculateHash();
    transactions.insert(transactions.begin(), coinbaseTx);

    Block newBlock;
    if (!chain.empty())
        newBlock = Block(chain.back().hash, "MedorCoin Block", medor, minerAddress);
    else
        newBlock = Block("", "Genesis Block", medor, minerAddress);

    newBlock.timestamp = time(nullptr);
    newBlock.transactions = transactions;
    newBlock.reward = reward;

    // Mine block
    mineBlock(newBlock);

    // Validate block before adding
    if (!chain.empty() && !validateBlock(newBlock, chain.back())) {
        std::cerr << "Invalid block, rejected!" << std::endl;
        return;
    }

    chain.push_back(newBlock);

    // Update UTXO set
    for (auto& tx : newBlock.transactions) {
        for (auto& in : tx.inputs) utxoSet.spendUTXO(in.prevTxHash, in.outputIndex);
        for (size_t i = 0; i < tx.outputs.size(); ++i)
            utxoSet.addUTXO(tx.outputs[i], tx.txHash, static_cast<int>(i));
    }

    totalSupply += reward;
}

// Validate a single block
bool Blockchain::validateBlock(const Block& block, const Block& previousBlock) {
    if (block.previousHash != previousBlock.hash) return false;
    if (block.timestamp <= previousBlock.timestamp) return false;

    // Validate transactions
    for (auto& tx : block.transactions) {
        if (tx.txHash.empty()) return false;
    }
    return true;
}

// Validate entire chain
bool Blockchain::validateChain() {
    for (size_t i = 1; i < chain.size(); ++i)
        if (!validateBlock(chain[i], chain[i - 1])) return false;
    return true;
}

// Get last block
Block Blockchain::getLastBlock() {
    std::lock_guard<std::mutex> lock(mtx);
    return chain.empty() ? Block() : chain.back();
}

// Print chain
void Blockchain::printChain() {
    for (auto& b : chain) {
        std::cout << "Block hash: " << b.hash << ", prev: " << b.previousHash
                  << ", timestamp: " << b.timestamp << std::endl;
    }
}
