#pragma once

#include <string>
#include "blockchain.h"
#include <nlohmann/json.hpp>

/**
 * SyncManager handles syncing missing blocks between peers.
 */
class SyncManager {
public:
    explicit SyncManager(Blockchain &chain);

    // Called when a sync_block message arrives
    void handleSyncBlock(const nlohmann::json &msg);

    // Called when a peer announces its latest chain length
    void handlePeerHeight(const std::string &peerId, uint64_t height);

private:
    Blockchain &chain;
};
