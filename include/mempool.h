#pragma once
#include <vector>
#include <string>
#include <cstdint>
#include <mutex>
#include "block.h"
#include "transaction.h"
#include "utxo.h"

class Blockchain {
public:
    Blockchain(const std::string& ownerAddr);

    // Add mined block
    void addBlock(const std::string& minerAddress, std::vector<Transaction>& transactions);

    // Validate a single block
    bool validateBlock(const Block& block, const Block& previousBlock);

    // Validate entire chain
    bool validateChain();

    // Fetch last block
    Block getLastBlock();

    // Print chain for debug
    void printChain();

    // Access UTXO set
    UTXOSet utxoSet;

private:
    std::vector<Block> chain;
    std::string ownerAddress;
    uint64_t medor;
    uint64_t totalSupply;
    uint64_t maxSupply;
    std::mutex mtx;

    // Mining reward
    uint64_t calculateReward();

    // Lightweight PoW
    void mineBlock(Block& block);
};
