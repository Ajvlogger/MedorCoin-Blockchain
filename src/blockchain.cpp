#include <iostream>
#include <ctime>
#include "blockchain.h"
#include "crypto.h"

using namespace std;

Blockchain::Blockchain(string ownerAddr) {
    ownerAddress = ownerAddr;
    medor = 0x1e00ffff;
    totalSupply = 0;
    maxSupply = 50000000;

    balances.clear();

    // Add genesis
    chain.push_back(createGenesisBlock());
}

Block Blockchain::createGenesisBlock() {
    Block genesis("0", "Genesis Block", medor, ownerAddress);
    genesis.timestamp = time(nullptr);

    genesis.reward = 100;
    totalSupply += genesis.reward;
    balances[ownerAddress] += genesis.reward;

    genesis.hash = doubleSHA256(genesis.headerToString());
    return genesis;
}

uint64_t Blockchain::calculateReward() {
    time_t now = time(nullptr);
    time_t genesisTime = chain[0].timestamp;
    double months = difftime(now, genesisTime) / (60*60*24*30.0);

    if (months <= 2.0) return 55;
    return 30;
}

void Blockchain::mineBlock(Block &block) {
    string target = "0";
    do {
        block.nonce++;
        block.hash = doubleSHA256(block.headerToString());
    } while (block.hash.substr(0, target.size()) != target);
}

void Blockchain::addBlock(const string &data, const string &minerAddr) {
    uint64_t reward = calculateReward();
    if (totalSupply + reward > maxSupply)
        reward = maxSupply - totalSupply;

    Block newBlock(chain.back().hash, data, medor, minerAddr);
    newBlock.timestamp = time(nullptr);
    newBlock.reward = reward;

    mineBlock(newBlock);
    chain.push_back(newBlock);

    uint64_t minerReward = (reward * 90) / 100;
    uint64_t ownerReward = reward - minerReward;

    balances[minerAddr] += minerReward;
    balances[ownerAddress] += ownerReward;
    totalSupply += reward;
}

void Blockchain::printChain() const {
    for (size_t i = 0; i < chain.size(); i++) {
        cout << "Block " << i << ":\n";
        cout << "  PrevHash: " << chain[i].previousHash << "\n";
        cout << "  Hash: " << chain[i].hash << "\n";
        cout << "  Reward: " << chain[i].reward << "\n";
        cout << "  Miner: " << chain[i].minerAddress << "\n\n";
    }
    cout << "=== Balances ===\n";
    for (auto &p : balances)
        cout << p.first << ": " << p.second << " MEDOR\n";
    cout << "Total: " << totalSupply << " MEDOR\n";
}
