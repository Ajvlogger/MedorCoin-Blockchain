#pragma once

#ifndef MEMPOOL_H
#define MEMPOOL_H

#include "transaction.h"
#include <unordered_map>
#include <vector>
#include <string>
#include <mutex>

class Mempool {
public:
    Mempool() = default;
    ~Mempool() = default;

    bool addTransaction(const Transaction &tx);
    void removeTransaction(const std::string &txHash);
    std::vector<Transaction> getTransactions();
    bool hasTransaction(const std::string &txHash);

private:
    std::unordered_map<std::string, Transaction> txMap;
    std::mutex mtx;
};

#endif // MEMPOOL_H
