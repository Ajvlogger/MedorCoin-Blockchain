#include "mempool.h"

bool Mempool::addTransaction(const Transaction& tx) {
    std::lock_guard<std::mutex> lock(mtx);
    if (txMap.find(tx.txHash) != txMap.end()) return false; // Already exists
    txMap[tx.txHash] = tx;
    return true;
}

void Mempool::removeTransaction(const std::string& txHash) {
    std::lock_guard<std::mutex> lock(mtx);
    txMap.erase(txHash);
}

std::vector<Transaction> Mempool::getTransactions() {
    std::lock_guard<std::mutex> lock(mtx);
    std::vector<Transaction> txs;
    for (auto& kv : txMap) txs.push_back(kv.second);
    return txs;
}

bool Mempool::hasTransaction(const std::string& txHash) {
    std::lock_guard<std::mutex> lock(mtx);
    return txMap.find(txHash) != txMap.end();
}
