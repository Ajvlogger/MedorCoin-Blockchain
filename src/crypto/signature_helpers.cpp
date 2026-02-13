#include "signature_helpers.h"

uint8_t computeEip155V(uint8_t recid, uint64_t chainId) {
    // EIP‑155 specifies: v = recid + 35 + 2 × chainId
    return static_cast<uint8_t>(recid + 35 + (chainId * 2));
}
