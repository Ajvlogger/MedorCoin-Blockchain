#pragma once
#include <string>
#include <vector>
#include <cstdint>

struct TxInput {
    std::string prevTxHash; // Hash of previous transaction
    int outputIndex;        // Which output of the prevTx
    std::string signature;  // Signed by sender
};

struct TxOutput {
    uint64_t value;         // Amount in MEDOR
    std::string address;    // Recipient
};

class Transaction {
public:
    std::string txHash;
    std::vector<TxInput> inputs;
    std::vector<TxOutput> outputs;

    void calculateHash(); // SHA256 hash of inputs + outputs
};
