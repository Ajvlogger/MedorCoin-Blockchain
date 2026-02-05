#include "mempool.h"

// Add a transaction to the mempool if it doesn’t already exist
bool Mempool::addTransaction(const Transaction &tx) {
    std::lock_guard<std::mutex> lock(mtx);
    // If transaction hash already exists, don’t add
    if (txMap.find(tx.txHash) != txMap.end()) {
        return false;
    }
    txMap[tx.txHash] = tx;
    return true;
}

// Remove a transaction by its hash
void Mempool::removeTransaction(const std::string &txHash) {
    std::lock_guard<std::mutex> lock(mtx);
    txMap.erase(txHash);
}

// Get all transactions currently in the mempool
std::vector<Transaction> Mempool::getTransactions() {
    std::lock_guard<std::mutex> lock(mtx);
    std::vector<Transaction> txs;
    txs.reserve(txMap.size());
    for (auto &kv : txMap) {
        txs.push_back(kv.second);
    }
    return txs;
}

// Check if a transaction exists in the mempool
bool Mempool::hasTransaction(const std::string &txHash) {
    std::lock_guard<std::mutex> lock(mtx);
    return txMap.find(txHash) != txMap.end();
}
