#include "transaction.h"
#include "crypto.h"

void Transaction::calculateHash() {
    std::string data;
    for(auto &in : inputs) data += in.prevTxHash + std::to_string(in.outputIndex) + in.signature;
    for(auto &out: outputs) data += out.address + std::to_string(out.value);
    txHash = doubleSHA256(data);
}
