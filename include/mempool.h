#pragma once
#include <vector>
#include <mutex>
#include <string>
#include <unordered_map>
#include "transaction.h"

// Thread-safe mempool for transaction propagation
class Mempool {
public:
    Mempool() = default;

    // Add transaction to mempool
    bool addTransaction(const Transaction& tx);

    // Remove transaction after mining
    void removeTransaction(const std::string& txHash);

    // Fetch all transactions ready to be mined
    std::vector<Transaction> getTransactions();

    // Check if a transaction exists
    bool hasTransaction(const std::string& txHash);

private:
    std::unordered_map<std::string, Transaction> txMap;
    std::mutex mtx; // thread-safe access
};
