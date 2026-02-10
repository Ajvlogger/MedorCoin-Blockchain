#include "miner.h"
#include "blockchain.h"
#include "mempool.h"
#include "transaction.h"
#include <vector>
#include <iostream>

// Mine a block with only reward + no mempool txs
void Miner::mineMedor(Blockchain &chain, const std::string &minerAddress) {
    std::cout << "[Miner] Starting mining with miner address: "
              << minerAddress << std::endl;

    // Prepare an empty transaction list
    std::vector<Transaction> txs;

    // Add block to chain (this will include coinbase reward)
    chain.addBlock(minerAddress, txs);

    std::cout << "[Miner] Block mined and added to chain." << std::endl;
}

// Mine including transactions from the mempool
void Miner::mineWithMempool(Blockchain &chain,
                            const std::string &minerAddress) {
    std::cout << "[Miner] Mining with mempool transactions." << std::endl;

    // Fetch pending transactions from mempool
    std::vector<Transaction> pendingTxs = Mempool::getPendingTxs();

    std::vector<Transaction> validTxs;
    validTxs.reserve(pendingTxs.size());

    // Validate + process each pending TX
    for (auto &tx : pendingTxs) {
        // Calculate TX ID
        tx.calculateHash();

        // Try to process the transaction with fees
        bool ok = processTransaction(tx, chain, minerAddress);
        if (ok) {
            validTxs.push_back(tx);
        } else {
            std::cout << "[Miner] Skipping invalid or unpaid TX: "
                      << tx.txHash << std::endl;
        }
    }

    // Now add block with valid transactions
    chain.addBlock(minerAddress, validTxs);

    std::cout << "[Miner] Block mined with "
              << validTxs.size() << " transactions." << std::endl;

    // Clear those transactions from mempool
    Mempool::removeConfirmed(validTxs);
}
