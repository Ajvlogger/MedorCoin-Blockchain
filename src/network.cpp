#include "network.h"
#include <iostream>

void Network::connectToPeer(const std::string& address) {
    peers.push_back(address);
    std::cout << "Connected to peer: " << address << std::endl;
}

void Network::broadcastTransaction(const Transaction& tx) {
    for (auto &peer : peers) {
        std::cout << "Sending transaction " << tx.txid << " to " << peer << std::endl;
    }
}

void Network::broadcastBlock(const Block& block) {
    for (auto &peer : peers) {
        std::cout << "Broadcasting block " << block.hash << " to " << peer << std::endl;
    }
}
