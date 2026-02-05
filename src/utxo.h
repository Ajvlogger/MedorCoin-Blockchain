#pragma once

#include <string>
#include <unordered_map>
#include <cstdint>

struct TxOutput {
    uint64_t value;
    std::string address;
};

struct UTXO {
    std::string txHash;
    int index;
    uint64_t value;
    std::string address;
};

class UTXOSet {
private:
    // Map key format: txHash:index -> UTXO
    std::unordered_map<std::string, UTXO> utxos;

    // Generate key for map
    std::string makeKey(const std::string& txHash, int index) const {
        return txHash + ":" + std::to_string(index);
    }

public:
    void addUTXO(const TxOutput& output, const std::string& txHash, int index);
    void spendUTXO(const std::string& txHash, int index);
    uint64_t getBalance(const std::string& address) const;

    bool isUnspent(const std::string& txHash, int index) const;
};
