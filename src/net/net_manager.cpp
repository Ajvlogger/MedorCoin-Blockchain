#include "net/net_manager.h"
#include <nlohmann/json.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <mutex>
#include <unordered_set>
#include <functional>

using json = nlohmann::json;

// Simple PeerManager stub
class PeerManager {
public:
    explicit PeerManager(const std::string &listenAddr)
        : listenAddress(listenAddr) {}

    bool start() {
        std::cout << "[Net] Listening on " << listenAddress << std::endl;
        return true;
    }

    bool connectTo(const std::string &peerAddr) {
        std::lock_guard<std::mutex> lock(mtx);
        peers.insert(peerAddr);
        std::cout << "[Net] Connected to " << peerAddr << std::endl;
        return true;
    }

    void broadcast(const json &msg) {
        std::string out = msg.dump();
        std::lock_guard<std::mutex> lock(mtx);
        for (auto &p : peers) {
            std::cout << "[Net] Sent to " << p << " : " << out << std::endl;
            if (onMsg) onMsg(msg);
        }
    }

    void setHandler(const std::function<void(const json &)> &handler) {
        onMsg = handler;
    }

private:
    std::string listenAddress;
    std::unordered_set<std::string> peers;
    std::mutex mtx;
    std::function<void(const json &)> onMsg;
};

NetworkManager::NetworkManager(const std::string &listenAddr)
    : peerMgr(nullptr) {
    peerMgr = new PeerManager(listenAddr);
}

NetworkManager::~NetworkManager() {
    if (peerMgr) {
        delete peerMgr;
        peerMgr = nullptr;
    }
}

bool NetworkManager::start() {
    if (peerMgr) return peerMgr->start();
    return false;
}

bool NetworkManager::connectBootstrap(const std::vector<std::string> &bootstrap) {
    if (!peerMgr) return false;
    for (auto &addr : bootstrap) {
        if (!peerMgr->connectTo(addr)) return false;
    }
    return true;
}

void NetworkManager::broadcastBlock(const nlohmann::json &blockMsg) {
    if (peerMgr) peerMgr->broadcast(blockMsg);
}

void NetworkManager::broadcastTx(const nlohmann::json &txMsg) {
    if (peerMgr) peerMgr->broadcast(txMsg);
}

void NetworkManager::onMessage(const std::function<void(const nlohmann::json &)> &handler) {
    if (peerMgr) peerMgr->setHandler(handler);
}
