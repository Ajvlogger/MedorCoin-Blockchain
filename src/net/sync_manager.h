#pragma once

#include <string>
#include <cstdint>
#include <nlohmann/json.hpp>
#include "blockchain.h"

/**
 * SyncManager — handles incoming sync messages and
 * triggers fork resolution when new blocks arrive.
 */
class SyncManager {
public:
    explicit SyncManager(Blockchain &chainRef);

    /**
     * Handle "sync_block" messages from peers.
     * Builds a candidate chain and runs fork resolution.
     */
    void handleSyncBlock(const nlohmann::json &msg);

    /**
     * Handle "announce_height" messages from peers.
     * If peer height is ahead, request missing blocks.
     */
    void handlePeerHeight(const std::string &peerId, uint64_t height);

private:
    Blockchain &chain;
};
