#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include <vector>
#include <string>
#include "block.h"
#include "transaction.h"

class Blockchain {
public:
    std::vector<Block> chain;
    uint32_t medor; // Difficulty

    Blockchain();

    Block createGenesisBlock();
    void addBlock(const std::vector<Transaction>& transactions);
};

#endif
