#include "net/sync_manager.h"
#include "net/serialization.h"
#include "net/net_manager.h"
#include "blockchain_fork.h"
#include <iostream>

SyncManager::SyncManager(Blockchain &chainRef)
    : chain(chainRef) {}

void SyncManager::handleSyncBlock(const nlohmann::json &msg) {
    if (msg.value("type", "") != "sync_block") return;

    Block blk = deserializeBlock(msg["block"]);

    std::vector<Block> candidate = chain.chain;
    candidate.push_back(blk);

    if (!chain.resolveFork(candidate)) {
        std::cout << "[SYNC] Received block ignored (not extending longest chain)" << std::endl;
    }
}

void SyncManager::handlePeerHeight(const std::string &peerId, uint64_t height) {
    uint64_t localHeight = chain.chain.size();
    if (height > localHeight) {
        nlohmann::json req;
        req["type"] = "sync_request";
        req["fromIndex"] = localHeight;

        NetworkManager mgr(""); // supply real listen address setup
        mgr.broadcastBlock(req);

        std::cout << "[SYNC] Requested blocks from height "
                  << localHeight << " from peer " << peerId << std::endl;
    }
}
