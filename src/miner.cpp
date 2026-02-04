#include "miner.h"
#include <iostream>

Miner::Miner(Blockchain& bc) : chain(bc) {}

void Miner::minePendingTransactions() {
    auto transactions = Mempool::getInstance().getTransactions();
    if (transactions.empty()) {
        std::cout << "No transactions to mine." << std::endl;
        return;
    }

    MerkleTree tree(transactions);
    std::string merkleRoot = tree.root;

    std::string txData;
    for (auto &tx : transactions) txData += tx.txid;

    Block block(chain.chain.back().hash, txData, chain.medor);
    chain.chain.push_back(block);

    Mempool::getInstance().clear();

    std::cout << "Block mined with Merkle root: " << merkleRoot << std::endl;
}
