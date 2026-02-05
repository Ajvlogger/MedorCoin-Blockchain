#include "transaction.h"
#include "crypto.h"
#include <vector>
#include <cstdint>
#include <string>

// Helper: write uint32_t as little-endian bytes
static void writeUInt32LE(std::vector<unsigned char>& buf, uint32_t value) {
    buf.push_back(value & 0xff);
    buf.push_back((value >> 8) & 0xff);
    buf.push_back((value >> 16) & 0xff);
    buf.push_back((value >> 24) & 0xff);
}

// Helper: write uint64_t as little-endian bytes
static void writeUInt64LE(std::vector<unsigned char>& buf, uint64_t value) {
    for (int i = 0; i < 8; ++i) {
        buf.push_back(value & 0xff);
        value >>= 8;
    }
}

// Serialize a transaction and compute its hash (Bitcoin-style)
void Transaction::calculateHash() {
    std::vector<unsigned char> serialized;

    // Version number (use 1)
    writeUInt32LE(serialized, 1);

    // Input count
    serialized.push_back(static_cast<unsigned char>(inputs.size()));

    // Serialize inputs
    for (auto& in : inputs) {
        // 32-byte prev tx hash (truncate/pad if needed)
        for (size_t i = 0; i < 32 && i < in.prevTxHash.size(); ++i)
            serialized.push_back(static_cast<unsigned char>(in.prevTxHash[i]));

        // Output index
        writeUInt32LE(serialized, in.outputIndex);

        // ScriptSig length (simplified, 0 if none)
        serialized.push_back(0);

        // Sequence (default 0xffffffff)
        writeUInt32LE(serialized, 0xffffffff);
    }

    // Output count
    serialized.push_back(static_cast<unsigned char>(outputs.size()));

    // Serialize outputs
    for (auto& out : outputs) {
        writeUInt64LE(serialized, out.value);

        // ScriptPubKey length (simplified, 0 if none)
        serialized.push_back(0);
    }

    // Locktime (default 0)
    writeUInt32LE(serialized, 0);

    // Compute double SHA-256 hash (Bitcoin-style TXID)
    txHash = doubleSHA256(std::string(serialized.begin(), serialized.end()));
}
