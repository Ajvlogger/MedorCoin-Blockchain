#include "evm_sign.h"
#include "rlp.h"
#include "keccak/keccak.h"
#include "signature.h"
#include "signature_helpers.h"
#include <secp256k1.h>
#include <secp256k1_recovery.h>
#include <fstream>

// Helper to load raw private key bytes (32 bytes) from a file
static std::vector<uint8_t> loadPrivKey(const std::string &path) {
    std::ifstream in(path, std::ios::binary);
    std::vector<uint8_t> key(32);
    in.read(reinterpret_cast<char*>(key.data()), 32);
    return key;
}

std::vector<uint8_t> signEvmTransaction(
    Transaction &tx,
    const std::string &privKeyPath,
    uint64_t chainId
) {
    using namespace rlp;

    // RLP encode unsigned tx fields
    auto encNonce    = encodeUInt(tx.nonce);
    auto encMaxFee   = encodeUInt(tx.maxFeePerGas);
    auto encTip      = encodeUInt(tx.maxPriorityFeePerGas);
    auto encGasLimit = encodeUInt(tx.gasLimit);
    std::vector<uint8_t> toBytes(tx.toAddress.begin(), tx.toAddress.end());
    auto encTo       = encodeBytes(toBytes);
    auto encValue    = encodeUInt(tx.value);
    auto encData     = encodeBytes(tx.data);

    // Build RLP list for signing
    std::vector<std::vector<uint8_t>> items = {
        encNonce,
        encMaxFee,
        encTip,
        encGasLimit,
        encTo,
        encValue,
        encData,
        encodeUInt(0), // v placeholder
        encodeUInt(0), // r placeholder
        encodeUInt(0)  // s placeholder
    };

    std::vector<uint8_t> raw = encodeList(items);

    // Keccak-256 the RLP to get the signing hash
    uint8_t hashOut[32];
    keccak(raw.data(), raw.size(), hashOut, 32);
    std::array<uint8_t,32> hashBytes;
    memcpy(hashBytes.data(), hashOut, 32);

    // Load private key from file
    auto privKey = loadPrivKey(privKeyPath);

    // Create recoverable signature
    secp256k1_context *ctx = secp256k1_context_create(SECP256K1_CONTEXT_SIGN);
    secp256k1_ecdsa_recoverable_signature sigRec;
    secp256k1_ecdsa_sign_recoverable(
        ctx,
        &sigRec,
        hashBytes.data(),
        privKey.data(),
        nullptr,
        nullptr
    );

    // Serialize signature to get r,s and recid
    unsigned char compactSig[64];
    int recid = 0;
    secp256k1_ecdsa_recoverable_signature_serialize_compact(
        ctx,
        compactSig,
        &recid,
        &sigRec
    );

    secp256k1_context_destroy(ctx);

    // Assign r and s
    std::array<uint8_t,32> rArr;
    std::array<uint8_t,32> sArr;
    memcpy(rArr.data(), compactSig, 32);
    memcpy(sArr.data(), compactSig + 32, 32);

    tx.r = rArr;
    tx.s = sArr;

    // Compute EIP‑155 v
    tx.v = computeEip155V(static_cast<uint8_t>(recid), chainId);

    // Now RLP encode with real v/r/s
    auto finalV = encodeUInt(tx.v);
    auto finalR = encodeBytes(std::vector<uint8_t>(tx.r.begin(), tx.r.end()));
    auto finalS = encodeBytes(std::vector<uint8_t>(tx.s.begin(), tx.s.end()));

    std::vector<std::vector<uint8_t>> finalItems = {
        encNonce,
        encMaxFee,
        encTip,
        encGasLimit,
        encTo,
        encValue,
        encData,
        finalV,
        finalR,
        finalS
    };

    return encodeList(finalItems);
}
