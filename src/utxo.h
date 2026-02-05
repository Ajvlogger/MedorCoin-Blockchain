#pragma once
#include "transaction.h"
#include <map>
#include <string>
#include <tuple>

class UTXOSet {
public:
    // key = txHash:index, value = TxOutput
    std::map<std::string, TxOutput> utxos;

    void addUTXO(const TxOutput& output, const std::string& txHash, int index) {
        utxos[txHash + ":" + std::to_string(index)] = output;
    }

    void spendUTXO(const std::string& txHash, int index) {
        utxos.erase(txHash + ":" + std::to_string(index));
    }
};
