#pragma once

#include <string>
#include <vector>
#include <cstdint>

// A transaction input references a previous UTXO and includes a signature
struct TxInput {
    std::string prevTxHash;   // The hash (ID) of the transaction being spent
    uint32_t outputIndex;     // Index of the specific output in that TX
    std::string signature;    // Signature unlocking that output
};

// A transaction output defines a new UTXO — value and destination address
struct TxOutput {
    uint64_t value;           // Amount in MedorCoin units
    std::string address;      // Bitcoin‑style Base58Check address
};

class Transaction {
public:
    uint32_t version = 1;                  // Transaction version (default 1)
    std::vector<TxInput> inputs;           // Input list
    std::vector<TxOutput> outputs;         // Output list
    uint32_t lockTime = 0;                 // Lock time (default 0)
    std::string txHash;                    // Double SHA‑256 TX identifier

    // Computes the transaction ID (Bitcoin‑style serialization + double SHA‑256)
    void calculateHash();
};
