#include "transaction.h"
#include "crypto/digest.h"          // keccak256
#include "evm/tx.h"
#include "blockchain.h"
#include "evm/storage.h"
#include "evm/execute.h"

#include <evmc/evmc.hpp>
#include <vector>
#include <cstring>
#include <iostream>

// -------------------------------
// Helpers for Little‑Endian Writes
// -------------------------------

static void writeUInt32LE(std::vector<uint8_t>& buf, uint32_t value) {
    buf.push_back(value & 0xff);
    buf.push_back((value >> 8) & 0xff);
    buf.push_back((value >> 16) & 0xff);
    buf.push_back((value >> 24) & 0xff);
}

static void writeUInt64LE(std::vector<uint8_t>& buf, uint64_t value) {
    for (int i = 0; i < 8; ++i) {
        buf.push_back(value & 0xff);
        value >>= 8;
    }
}

// -------------------------------
// UTXO Transaction Serialization
// -------------------------------

static void serializeUTXO(const Transaction& tx, std::vector<uint8_t>& out) {
    writeUInt32LE(out, tx.version);
    out.push_back(static_cast<uint8_t>(tx.inputs.size()));

    for (const auto& in : tx.inputs) {
        out.insert(out.end(), in.prevTxHash.begin(), in.prevTxHash.end());
        writeUInt32LE(out, in.outputIndex);
        writeUInt32LE(out, in.sequence);
    }

    out.push_back(static_cast<uint8_t>(tx.outputs.size()));
    for (const auto& o : tx.outputs) {
        writeUInt64LE(out, o.value);
        // scriptPubKey placeholder
        out.push_back(0);
    }

    writeUInt32LE(out, tx.lockTime);
}

// -------------------------------
// EVM Transaction Serialization
// -------------------------------

static void serializeEVM(const Transaction& tx, std::vector<uint8_t>& out) {
    evm::Tx signedTx;
    signedTx.nonce      = tx.evmNonce;
    signedTx.gasPrice   = tx.maxFeePerGas;
    signedTx.gasLimit   = tx.gasLimit;
    signedTx.value      = tx.value;
    signedTx.to         = tx.toAddress;
    signedTx.data       = tx.data;

    // RLP serialization happens in real signing function
    // Placeholder insert raw
    out.insert(out.end(), tx.toAddress.begin(), tx.toAddress.end());
    writeUInt64LE(out, signedTx.value);
    writeUInt64LE(out, signedTx.gasPrice);
    writeUInt64LE(out, signedTx.gasLimit);

    out.insert(out.end(), signedTx.data.begin(), signedTx.data.end());
}

// -------------------------------
// Compute Transaction Hash
// -------------------------------

void Transaction::calculateHash() {
    std::vector<uint8_t> raw;

    if (type == TxType::Standard) {
        serializeUTXO(*this, raw);
        // UTXO uses double SHA‑256
        txHash = doubleSHA256(raw);
    } else {
        // EVM transaction uses KECCAK‑256 of RLP
        serializeEVM(*this, raw);
        uint8_t hashOut[32];
        keccak(raw.data(), raw.size(), hashOut, 32);
        txHash.assign(hashOut, hashOut + 32);
    }
}

// -------------------------------
// UTXO Processing
// -------------------------------

static bool processStandardTx(const Transaction& tx, Blockchain& chain) {
    std::cout << "[TX] Standard UTXO processed: " << tx.txHash << std::endl;
    return true;
}

// -------------------------------
// EVM Contract Deploy
// -------------------------------

static bool processContractDeploy(const Transaction& tx, Blockchain& chain, const std::string& minerAddr) {
    EVMStorage storage(chain.getEVMStatePath());

    std::string seed = tx.fromAddress + tx.txHash;
    std::vector<uint8_t> bytecode = tx.data;
    std::string contractAddress = bytesToHex(keccak256(seed));

    storage.putContractCode(contractAddress, bytecode);

    std::cout << "[EVM] Deployed at: " << contractAddress << std::endl;
    return true;
}

// -------------------------------
// EVM Contract Call + Fees
// -------------------------------

static bool processContractCall(const Transaction& tx, Blockchain& chain, const std::string& minerAddr) {
    EVMStorage storage(chain.getEVMStatePath());
    auto bytecode = storage.getContractCode(tx.toAddress);

    if (bytecode.empty()) {
        std::cerr << "[EVM] Contract not found: " << tx.toAddress << std::endl;
        return false;
    }

    evmc_address from{};
    evmc_address to{};
    std::memcpy(from.bytes, tx.fromAddress.data(), from.bytes.size());
    std::memcpy(to.bytes,   tx.toAddress.data(),   to.bytes.size());

    auto result = EVMExecutor::executeContract(
        storage, bytecode, tx.data, tx.gasLimit, to, from, chain, minerAddr
    );

    uint64_t gasUsed = tx.gasLimit - result.gas_left;
    uint64_t baseFee = chain.getCurrentBaseFee();
    uint64_t tip     = tx.maxPriorityFeePerGas;
    uint64_t effPrice = std::min(tx.maxFeePerGas, baseFee + tip);

    uint64_t totalFee = gasUsed * effPrice;

    uint64_t senderBalance = chain.getBalance(tx.fromAddress);
    if (senderBalance < totalFee) {
        std::cerr << "[FEE] Insufficient balance: needed "
                  << totalFee << ", available " << senderBalance << std::endl;
        return false;
    }

    chain.setBalance(tx.fromAddress, senderBalance - totalFee);
    chain.addBalance(minerAddr, gasUsed * tip);
    chain.burnBaseFees(gasUsed * baseFee);
    chain.currentBlock.gasUsed += gasUsed;

    std::cout << "[FEE] Charged: GasUsed=" << gasUsed
              << ", Total=" << totalFee << std::endl;

    return (result.status_code == EVMC_SUCCESS);
}

// -------------------------------
// Dispatch
// -------------------------------

bool processTransaction(const Transaction& tx, Blockchain& chain, const std::string& minerAddress) {
    switch (tx.type) {
        case TxType::Standard:
            return processStandardTx(tx, chain);
        case TxType::ContractDeploy:
            return processContractDeploy(tx, chain, minerAddress);
        case TxType::ContractCall:
            return processContractCall(tx, chain, minerAddress);
        default:
            std::cerr << "[TX] Unknown type" << std::endl;
            return false;
    }
}

// -------------------------------
// Debug Printer
// -------------------------------

void printTransaction(const Transaction& tx) {
    std::cout << "Tx Hash: " << tx.txHash << std::endl;
    std::cout << "Type: " << static_cast<int>(tx.type);

    if (tx.type != TxType::Standard) {
        std::cout << " From=" << tx.fromAddress
                  << " To=" << tx.toAddress
                  << " GasLimit=" << tx.gasLimit
                  << " MaxFee=" << tx.maxFeePerGas
                  << " PriorityFee=" << tx.maxPriorityFeePerGas
                  << " DataSize=" << tx.data.size();
    }
    std::cout << std::endl;
}
