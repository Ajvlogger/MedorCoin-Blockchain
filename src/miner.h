#ifndef MINER_H
#define MINER_H

#include "blockchain.h"
#include "mempool.h"
#include "merkle.h"

class Miner {
public:
    Blockchain& chain;

    Miner(Blockchain& bc);

    void minePendingTransactions();
};

#endif
