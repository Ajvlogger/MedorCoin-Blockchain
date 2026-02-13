#include "net/net_manager.h"
#include <nlohmann/json.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <mutex>
#include <thread>
#include <functional>
#include <unordered_set>

using json = nlohmann::json;

// Basic peer representation
struct Peer {
    std::string address;
    bool connected = false;
};

class PeerManager {
public:
    explicit PeerManager(const std::string &listenAddr)
        : listenAddress(listenAddr) {}

    bool start() {
        // Start listener thread (placeholder)
        std::cout << "[Net] Listening on " << listenAddress << std::endl;
        listening = true;
        return true;
    }

    bool connectTo(const std::string &peerAddr) {
        std::lock_guard<std::mutex> lock(peerMutex);
        peers.insert(peerAddr);
        std::cout << "[Net] Connected to peer " << peerAddr << std::endl;
        return true;
    }

    void broadcast(const json &msg) {
        std::lock_guard<std::mutex> lock(peerMutex);
        std::string serialized = msg.dump();
        for (auto &p : peers) {
            // In a real implementation, send over socket
            std::cout << "[Net] Broadcast to " << p << ": " << serialized << std::endl;
            // placeholder: simulate receive on all peers
            if (onMessageHandler) onMessageHandler(msg);
        }
    }

    void setHandler(const std::function<void(const json &)> &handler) {
        onMessageHandler = handler;
    }

private:
    std::string listenAddress;
    bool listening = false;
    std::mutex peerMutex;
    std::unordered_set<std::string> peers;
    std::function<void(const json &)> onMessageHandler;
};

// ------------ NetworkManager Implementation ------------

NetworkManager::NetworkManager(const std::string &listenAddr)
    : peerMgr(nullptr) {
    peerMgr = new PeerManager(listenAddr);
}

NetworkManager::~NetworkManager() {
    if (peerMgr != nullptr) {
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

void NetworkManager::broadcastBlock(const json &blockMsg) {
    if (peerMgr) peerMgr->broadcast(blockMsg);
}

void NetworkManager::broadcastTx(const json &txMsg) {
    if (peerMgr) peerMgr->broadcast(txMsg);
}

void NetworkManager::onMessage(const std::function<void(const json &)> &handler) {
    if (peerMgr) peerMgr->setHandler(handler);
}
