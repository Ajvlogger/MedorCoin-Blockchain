#include <iostream>
#include "blockchain.h"
#include "wallet.h"
#include "miner.h"
#include "mempool.h"
#include "network.h"
#include "config.h"

int main() {
    std::cout << "Starting " << COIN_NAME << " (" << COIN_SYMBOL << ")" << std::endl;

    // Initialize blockchain
    Blockchain medorChain;

    // Create wallets
    Wallet alice;
    Wallet bob;

    std::cout << "Alice address: " << alice.address << std::endl;
    std::cout << "Bob address: " << bob.address << std::endl;

    // Alice sends some Medor to Bob
    Transaction tx1 = alice.createTransaction(bob.address, 50);
    Mempool::getInstance().addTransaction(tx1);

    // Mining pending transactions
    Miner miner(medorChain);
    miner.minePendingTransactions();

    // Validate blockchain
    if (Consensus::validateChain(medorChain)) {
        std::cout << "Blockchain is valid!" << std::endl;
    } else {
        std::cout << "Blockchain is invalid!" << std::endl;
    }

    return 0;
}
