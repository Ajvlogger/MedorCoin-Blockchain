#include "transaction.h"
#include "crypto/rlp.h"
#include "crypto/keccak/keccak.h"

#include <sstream>
#include <iomanip>

static std::string toHex(const std::vector<uint8_t> &bytes) {
    std::ostringstream ss;
    ss << std::hex << std::setfill('0');
    for (auto b : bytes) {
        ss << std::setw(2) << (int)b;
    }
    return ss.str();
}

void Transaction::calculateHash() {
    using namespace rlp;

    // RLP encode fully signed EVM transaction
    auto encNonce    = encodeUInt(nonce);
    auto encMaxFee   = encodeUInt(maxFeePerGas);
    auto encTip      = encodeUInt(maxPriorityFeePerGas);
    auto encGasLimit = encodeUInt(gasLimit);
    std::vector<uint8_t> toBytes(toAddress.begin(), toAddress.end());
    auto encTo       = encodeBytes(toBytes);
    auto encValue    = encodeUInt(value);
    auto encData     = encodeBytes(data);
    auto encV        = encodeUInt(v);
    auto encR        = encodeBytes(std::vector<uint8_t>(r.begin(), r.end()));
    auto encS        = encodeBytes(std::vector<uint8_t>(s.begin(), s.end()));

    std::vector<std::vector<uint8_t>> items = {
        encNonce,
        encMaxFee,
        encTip,
        encGasLimit,
        encTo,
        encValue,
        encData,
        encV,
        encR,
        encS
    };

    std::vector<uint8_t> raw = encodeList(items);

    // Keccak‑256 hash
    uint8_t hashOut[32];
    keccak(raw.data(), raw.size(), hashOut, 32);
    std::vector<uint8_t> hashBytes(hashOut, hashOut + 32);

    // Convert to hex string for the stored txHash
    txHash = toHex(hashBytes);
}
