#include "net/net_manager.h"
#include "blockchain.h"
#include "transaction.h"
#include <nlohmann/json.hpp>
#include <iostream>

int main() {
    Blockchain medorChain("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
    NetworkManager net("127.0.0.1:4001");

    if (!net.start()) {
        std::cerr << "[Network] Failed\n";
        return -1;
    }

    // Handle incoming messages
    net.onMessage([&](const nlohmann::json &msg) {
        if (msg["type"] == "poa_block") {
            std::cout << "[Network] Received PoA block message\n";
            nlohmann::json b = msg["block"];
            Block block = deserializeBlock(b);

            // Validate PoA signature before accepting
            if (verifyBlockPoA(block)) {
                std::cout << "[Network] Valid PoA block signature\n";
                medorChain.addBlockFromPeer(block);
            } else {
                std::cerr << "[Network] Invalid PoA block signature\n";
            }
        } else if (msg["type"] == "tx") {
            // Deserialize and add to mempool
            Transaction tx = deserializeTx(msg["tx"]);
            globalMempool.addTransaction(tx);
        }
    });

    std::cout << "[Network] Listening…\n";
    while (true) {
        // Poll loop for network events
    }
}
