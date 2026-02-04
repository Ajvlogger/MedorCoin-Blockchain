#pragma once
#include <string>
#include <vector>
#include <ctime>
#include "transaction.h"

class Block {
public:
    std::string previousHash;
    std::string hash;
    std::string data;

    uint32_t medor;          // difficulty
    uint64_t nonce;          // PoW counter
    uint64_t reward;         // block reward
    time_t timestamp;

    std::string minerAddress;

    std::vector<Transaction> transactions;

    Block() : medor(0), nonce(0), reward(0), timestamp(0) {}

    Block(const std::string& prevHash,
          const std::string& d,
          uint32_t diff,
          const std::string& miner)
        : previousHash(prevHash),
          data(d),
          medor(diff),
          nonce(0),
          reward(0),
          timestamp(time(nullptr)),
          minerAddress(miner) {}

    std::string headerToString() const {
        std::string txData;
        for (const auto& tx : transactions) {
            txData += tx.txHash;
        }

        return previousHash
             + data
             + txData
             + std::to_string(timestamp)
             + std::to_string(nonce)
             + std::to_string(reward)
             + minerAddress;
    }
};
