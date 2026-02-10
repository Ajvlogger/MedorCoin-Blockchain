#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <evmc/evmc.hpp>

// -------------------------------
// UTXO Transaction Structures
// -------------------------------

struct TxInput {
    std::string prevTxHash;   // Hash of previous transaction
    uint32_t outputIndex;     // Index of the output being spent
    std::string signature;    // Signature unlocking the UTXO
};

struct TxOutput {
    uint64_t value;           // Amount in MedorCoin units
    std::string address;      // Destination address
};

// -------------------------------
// Transaction Type Enum
// -------------------------------

enum class TxType {
    Standard,        // Standard UTXO transfer
    ContractDeploy,  // EVM smart contract deployment
    ContractCall     // EVM smart contract call
};

// -------------------------------
// Main Transaction Class
// -------------------------------

class Transaction {
public:
    uint32_t version = 1;
    TxType type = TxType::Standard;

    // UTXO fields
    std::vector<TxInput> inputs;
    std::vector<TxOutput> outputs;

    // EVM fields
    std::string fromAddress;        // Sender (hex or raw bytes)
    std::string toAddress;          // Contract address (empty for deploy)
    std::vector<uint8_t> data;      // Bytecode (deploy) or calldata (call)
    uint64_t gasLimit = 0;

    uint32_t lockTime = 0;
    std::string txHash;

    // Compute tx hash
    void calculateHash();
};

// -------------------------------
// Transaction Processing API
// -------------------------------

// Dispatches transaction execution based on type
bool processTransaction(const Transaction& tx);

// Debug helper
void printTransaction(const Transaction& tx);
