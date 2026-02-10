#include "mempool.h"
#include <algorithm>
#include <iostream>

Mempool::Mempool() {
    // Optional RocksDB init:
    // mempoolDB = new MempoolDB("data/medorcoin_mempool");
}

Mempool::~Mempool() {
    // Optional RocksDB cleanup:
    // delete mempoolDB;
}

bool Mempool::addTransaction(const Transaction &tx) {
    std::lock_guard<std::mutex> lock(mtx);

    if (txMap.find(tx.txHash) != txMap.end())
        return false;

    txMap[tx.txHash] = tx;

    // Optional RocksDB persistence:
    // mempoolDB->put(tx.txHash, serialize(tx));

    return true;
}

void Mempool::removeTransaction(const std::string &txHash) {
    std::lock_guard<std::mutex> lock(mtx);
    txMap.erase(txHash);

    // Optional RocksDB remove:
    // mempoolDB->del(txHash);
}

std::vector<Transaction> Mempool::getTransactions() const {
    std::lock_guard<std::mutex> lock(mtx);
    std::vector<Transaction> txs;
    txs.reserve(txMap.size());
    for (auto &kv : txMap)
        txs.push_back(kv.second);
    return txs;
}

bool Mempool::hasTransaction(const std::string &txHash) const {
    std::lock_guard<std::mutex> lock(mtx);
    return (txMap.find(txHash) != txMap.end());
}

void Mempool::removeConfirmed(const std::vector<Transaction> &txs) {
    std::lock_guard<std::mutex> lock(mtx);
    for (const auto &tx : txs)
        txMap.erase(tx.txHash);

    // Optional RocksDB remove loop:
    // for (const auto &tx : txs) mempoolDB->del(tx.txHash);
}

std::vector<Transaction> Mempool::getSortedByFee(uint64_t currentBaseFee) const {
    std::lock_guard<std::mutex> lock(mtx);

    std::vector<Transaction> list;
    list.reserve(txMap.size());
    for (auto &kv : txMap)
        list.push_back(kv.second);

    // Sort descending by effective fee
    std::sort(list.begin(), list.end(),
              [&](const Transaction &a, const Transaction &b) {
        // effective price = min(maxFeePerGas, baseFee + priority)
        uint64_t feeA = std::min(a.maxFeePerGas,
                                 currentBaseFee + a.maxPriorityFeePerGas);
        uint64_t feeB = std::min(b.maxFeePerGas,
                                 currentBaseFee + b.maxPriorityFeePerGas);
        return feeA > feeB;
    });

    return list;
}
