#include "utxo.h"

// Add a new UTXO to the set
void UTXOSet::addUTXO(const TxOutput& output, const std::string& txHash, int index) {
    std::string key = makeKey(txHash, index);
    utxos[key] = UTXO{txHash, index, output.value, output.address};
}

// Remove a spent UTXO from the set
void UTXOSet::spendUTXO(const std::string& txHash, int index) {
    std::string key = makeKey(txHash, index);
    auto it = utxos.find(key);
    if (it != utxos.end()) {
        utxos.erase(it);
    }
}

// Get total balance for a specific address
uint64_t UTXOSet::getBalance(const std::string& address) const {
    uint64_t balance = 0;
    for (const auto& [key, utxo] : utxos) {
        if (utxo.address == address) {
            balance += utxo.value;
        }
    }
    return balance;
}

// Check if a specific UTXO is unspent
bool UTXOSet::isUnspent(const std::string& txHash, int index) const {
    std::string key = makeKey(txHash, index);
    return utxos.find(key) != utxos.end();
}
