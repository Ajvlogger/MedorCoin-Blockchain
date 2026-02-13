#pragma once

#include <vector>
#include <string>
#include <cstdint>
#include <array>
#include "block.h"
#include "transaction.h"
#include "utxo.h"
#include "db/blockdb.h"
#include "db/accountdb.h"

class Blockchain {
public:
    Blockchain(const std::string &ownerAddr);

    std::vector<Block> chain;
    UTXOSet utxoSet;

    std::string ownerAddress;
    uint32_t medor;
    uint64_t totalSupply;
    uint64_t maxSupply;

    BlockDB blockDB;
    AccountDB accountDB;

    uint64_t getCurrentBaseFee() const;
    void setCurrentBaseFee(uint64_t fee);
    void burnBaseFees(uint64_t amount);
    void adjustBaseFee(uint64_t gasUsed, uint64_t gasLimit);

    uint64_t getBalance(const std::string &addr) const;
    void setBalance(const std::string &addr, uint64_t amount);
    void addBalance(const std::string &addr, uint64_t amount);

    uint64_t getBalance(const std::array<uint8_t,20> &addr) const;
    void setBalance(const std::array<uint8_t,20> &addr, uint64_t amount);
    void addBalance(const std::array<uint8_t,20> &addr, uint64_t amount);

    bool verifyTransactionSignature(const Transaction &tx) const;
    bool executeTransaction(const Transaction &tx, const std::string &minerAddress);
    std::vector<Transaction> getTransactions(const std::vector<std::string> &hashes) const;

    uint64_t calculateReward();
    void mineBlock(Block &block);

    void addBlock(const std::string &minerAddress,
                  std::vector<Transaction> &transactions);

    bool validateBlock(const Block &block, const Block &previousBlock);
    bool validateChain();
    void printChain() const;

private:
    uint64_t baseFeePerGas = 1;
};
