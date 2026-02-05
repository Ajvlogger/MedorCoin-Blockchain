#include "blockchain.h"
#include "block.h"
#include "transaction.h"
#include "utxo.h"
#include <ctime>
#include <string>
#include <vector>

// Constructor
Blockchain::Blockchain(const std::string& ownerAddr) {
    ownerAddress = ownerAddr;
    medor = 0x1e00ffff;
    totalSupply = 0;
    maxSupply = 50000000;
}

// Calculate reward
unsigned long Blockchain::calculateReward() {
    time_t now = time(nullptr);
    time_t genesis = chain.empty() ? now : chain.front().timestamp;
    double months = difftime(now, genesis) / (60 * 60 * 24 * 30.0);
    return (months <= 2.0) ? 55 : 30;
}

// Lightweight mining (dummy PoW)
void Blockchain::mineBlock(Block& block) {
    block.hash = block.headerToString(); // Placeholder hash
}

// Add a new block
void Blockchain::addBlock(const std::string& minerAddress,
                          std::vector<Transaction>& transactions) {

    unsigned long reward = calculateReward();
    if (totalSupply + reward > maxSupply)
        reward = maxSupply - totalSupply;

    // Create coinbase transaction
    Transaction coinbaseTx;
    TxOutput minerOut, ownerOut;

    minerOut.value = (reward * 90) / 100;
    minerOut.address = minerAddress;

    ownerOut.value = reward - minerOut.value;
    ownerOut.address = ownerAddress;

    coinbaseTx.outputs.push_back(minerOut);
    coinbaseTx.outputs.push_back(ownerOut);
    coinbaseTx.calculateHash();

    transactions.insert(transactions.begin(), coinbaseTx);

    // Create new block
    Block newBlock;
    if (!chain.empty())
        newBlock = Block(chain.back().hash, "MedorCoin Block", medor, minerAddress);
    else
        newBlock = Block("", "Genesis Block", medor, minerAddress);

    newBlock.timestamp = time(nullptr);
    newBlock.reward = reward;
    newBlock.transactions = transactions;

    // Mine the block
    mineBlock(newBlock);

    // Add block to chain
    chain.push_back(newBlock);

    // Update UTXO set
    for (auto& tx : newBlock.transactions) {
        for (auto& in : tx.inputs)
            utxoSet.spendUTXO(in.prevTxHash, in.outputIndex);

        for (size_t i = 0; i < tx.outputs.size(); ++i)
            utxoSet.addUTXO(tx.outputs[i], tx.txHash, static_cast<int>(i));
    }

    totalSupply += reward;
}
