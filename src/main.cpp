#include "blockchain.h"
#include "transaction.h"
#include "net/net_manager.h"
#include "consensus/validator_registry.h"

#include <iostream>
#include <vector>
#include <string>

int main() {
    // -------------------------------
    // 1) Load PoA validator registry
    // -------------------------------
    ValidatorRegistry::loadValidators();
    std::cout << "[PoA] Loaded " 
              << ValidatorRegistry::getValidators().size() 
              << " validators\n";

    // -------------------------------
    // 2) Create blockchain instance
    // -------------------------------
    std::string ownerAddress = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"; // must be a validator
    Blockchain medorChain(ownerAddress);

    // -------------------------------
    // 3) Start network manager
    // -------------------------------
    NetworkManager net("127.0.0.1:4001");

    if (!net.start()) {
        std::cerr << "[Network] Failed to start\n";
        return -1;
    }

    // -------------------------------
    // 4) Connect bootstrap peers
    // -------------------------------
    std::vector<std::string> bootstrap = {
        "127.0.0.1:4002",
        "127.0.0.1:4003"
    };
    net.connectBootstrap(bootstrap);

    std::cout << "[Network] Node started and connected to bootstrap peers\n";

    // -------------------------------
    // 5) Example broadcast (placeholder)
    // -------------------------------
    std::string dummyBlockHex = "01020304deadbeef";
    std::string dummyTxHex    = "af23be4c";

    net.broadcastBlock(dummyBlockHex);
    net.broadcastTx(dummyTxHex);

    // -------------------------------
    // 6) Main loop placeholder
    // -------------------------------
    while (true) {
        // In a real node, handle network messages, new txs, and block production
        // For PoA, check if this node is validator, then produce block
        // sleep(1) or async wait...
    }

    return 0;
}
