#include "mempool.h"

Mempool& Mempool::getInstance() {
    static Mempool instance;
    return instance;
}

void Mempool::addTransaction(const Transaction& tx) {
    transactions.push_back(tx);
}

std::vector<Transaction> Mempool::getTransactions() {
    return transactions;
}

void Mempool::clear() {
    transactions.clear();
}
