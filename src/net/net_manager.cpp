#include "net/net_manager.h"

NetworkManager::NetworkManager(const std::string &listenAddr)
    : peerMgr(listenAddr) {}

bool NetworkManager::start() {
    return peerMgr.start();
}

bool NetworkManager::connectBootstrap(const std::vector<std::string> &bootstrap) {
    for (auto &addr : bootstrap) {
        if (!peerMgr.connectTo(addr)) return false;
    }
    return true;
}

void NetworkManager::broadcastBlock(const std::string &blockHex) {
    peerMgr.broadcast("BLOCK", blockHex);
}

void NetworkManager::broadcastTx(const std::string &txHex) {
    peerMgr.broadcast("TX", txHex);
}
