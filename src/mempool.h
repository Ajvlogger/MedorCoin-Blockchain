#ifndef MEMPOOL_H
#define MEMPOOL_H

#include <vector>
#include "transaction.h"

class Mempool {
public:
    static Mempool& getInstance();

    void addTransaction(const Transaction& tx);
    std::vector<Transaction> getTransactions();
    void clear();

private:
    std::vector<Transaction> transactions;
    Mempool() = default;
};

#endif
