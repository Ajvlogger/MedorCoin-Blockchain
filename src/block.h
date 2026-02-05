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
    uint32_t medor;        // difficulty
    uint64_t nonce;
    uint64_t reward;
    time_t timestamp;
    std::string minerAddress;
    std::vector<Transaction> transactions;

    Block(const std::string& prevHash = "",
          const std::string& d = "",
          uint32_t diff = 0,
          const std::string& miner = "",
          uint64_t rw = 0)
        : previousHash(prevHash),
          data(d),
          medor(diff),
          nonce(0),
          reward(rw),
          timestamp(time(nullptr)),
          minerAddress(miner) {}

    std::string headerToString() const {
        // Simple string representation for hashing
        return previousHash + data + std::to_string(medor) +
               std::to_string(nonce) + minerAddress;
    }
};
