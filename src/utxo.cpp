#include "utxo.h"
#include <algorithm>

UTXOSet& UTXOSet::getInstance() {
    static UTXOSet instance;
    return instance;
}

void UTXOSet::addUTXO(const UTXO& utxo) {
    utxos.push_back(utxo);
}

void UTXOSet::removeUTXO(const std::string& txid, uint32_t index) {
    utxos.erase(std::remove_if(utxos.begin(), utxos.end(),
        [&](const UTXO& u){ return u.txid == txid && u.index == index; }),
        utxos.end());
}

std::vector<UTXO> UTXOSet::getUTXOs(const std::string& address) {
    std::vector<UTXO> results;
    for (auto &u : utxos) {
        if (u.owner == address) results.push_back(u);
    }
    return results;
}
