#include "blockchain.h"
#include "transaction.h"
#include "net/net_manager.h"
#include <iostream>
#include <vector>

int main() {
    Blockchain medorChain;

    NetworkManager net("127.0.0.1/tcp/4001");
    if (!net.start()) {
        std::cerr << "Network failed\n";
        return -1;
    }

    std::vector<std::string> bootstrap = {
        "/ip4/127.0.0.1/tcp/4002",
        "/ip4/127.0.0.1/tcp/4003"
    };
    net.connectBootstrap(bootstrap);

    std::cout << "MedorCoin node started and connected\n";

    // Example broadcast (in hex)
    net.broadcastBlock("01020304deadbeef");
    net.broadcastTx("af23be4c");

    return 0;
}
