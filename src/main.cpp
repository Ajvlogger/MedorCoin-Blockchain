#include <iostream>
#include <string>
#include "blockchain.h"
#include "miner.h"
#include "transaction.h"  // include transactions so txHash exists
#include "block.h"

int main() {

    // Create MedorCoin blockchain with owner address
    std::string owner = "OWNER_ADDR_ABC";
    Blockchain medorChain(owner);

    // Create Miner
    Miner miner;

    // Mine some blocks
    miner.mineMedor(medorChain, "MINER_1");
    miner.mineMedor(medorChain, "MINER_2");
    miner.mineMedor(medorChain, "MINER_1");

    // Print blockchain
    std::cout << "---- MedorCoin Blockchain ----" << std::endl;

    for (size_t i = 0; i < medorChain.chain.size(); ++i) {
        const Block& blk = medorChain.chain[i];
        std::cout << "Block #" << i << std::endl;
        std::cout << "  Previous Hash: " << blk.previousHash << std::endl;
        std::cout << "  Hash         : " << blk.hash << std::endl;
        std::cout << "  Miner        : " << blk.minerAddress << std::endl;
        std::cout << "  Timestamp    : " << blk.timestamp << std::endl;
        std::cout << "  Reward       : " << blk.reward << std::endl;
        std::cout << "  Transactions : " << blk.transactions.size() << std::endl;
        for (size_t j = 0; j < blk.transactions.size(); ++j) {
            const Transaction& tx = blk.transactions[j];
            std::cout << "    Tx " << j << ": " << tx.txHash << std::endl;
        }
        std::cout << "-------------------------------" << std::endl;
    }

    return 0;
}
