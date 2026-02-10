#pragma once

#include <vector>
#include <string>
#include <cstdint>
#include <unordered_map>
#include "block.h"
#include "transaction.h"
#include "utxo.h"

class Blockchain {
public:
    Blockchain(const std::string &ownerAddr);

    // Chain storage
    std::vector<Block> chain;
    UTXOSet utxoSet;
    std::string ownerAddress;
    uint32_t medor;            // PoW difficulty
    uint64_t totalSupply;
    uint64_t maxSupply;

    // Base fee support
    uint64_t getCurrentBaseFee() const;
    void setCurrentBaseFee(uint64_t fee);

    // Account state (balances)
    uint64_t getBalance(const std::string &addr) const;
    void setBalance(const std::string &addr, uint64_t amount);
    void addBalance(const std::string &addr, uint64_t amount);

    // Burn base fees (protocol treasury / deflation)
    void burnBaseFees(uint64_t amount);

    // Block reward + mining
    uint64_t calculateReward();
    void mineBlock(Block &block);

    // Add block with transactions
    void addBlock(const std::string &minerAddress,
                  std::vector<Transaction> &transactions);

    // Chain validation
    bool validateBlock(const Block &block, const Block &previousBlock);
    bool validateChain();

    // Optional debug dump
    void printChain() const;

    // Base fee update helper
    void adjustBaseFee(uint64_t gasUsed, uint64_t gasLimit);

private:
    class BlockDB blockDB;
    // Base fee per gas for the next block
    uint64_t baseFeePerGas = 1;  // start with 1 unit

    // Account balances (in memory)
    mutable std::unordered_map<std::string, uint64_t> balanceMap;
};
