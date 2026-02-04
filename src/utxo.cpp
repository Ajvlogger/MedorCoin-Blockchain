#include "utxo.h"

void UTXOSet::addUTXO(const TxOutput &output, const std::string &txHash, int index) {
    UTXO utxo{txHash, index, output.value, output.address};
    utxos[txHash + ":" + std::to_string(index)] = utxo;
}

void UTXOSet::spendUTXO(const std::string &txHash, int index) {
    utxos.erase(txHash + ":" + std::to_string(index));
}

uint64_t UTXOSet::getBalance(const std::string &address) {
    uint64_t sum = 0;
    for(auto &pair : utxos) {
        if(pair.second.address == address) sum += pair.second.value;
    }
    return sum;
}
