#include "recovery.h"
#include <secp256k1.h>
#include <secp256k1_recovery.h>
#include <cstring>

// Try all possible recovery IDs and return the one that recovers a key matching expectedPubkey
int findRecoveryId(
    const std::array<uint8_t,32> &hash,
    const std::array<uint8_t,32> &r,
    const std::array<uint8_t,32> &s,
    std::array<uint8_t,65> &outPubkey
) {
    secp256k1_context *ctx = secp256k1_context_create(SECP256K1_CONTEXT_SIGN | SECP256K1_CONTEXT_VERIFY);

    // Create recoverable signature from r and s
    secp256k1_ecdsa_recoverable_signature sig;
    unsigned char compactSig[64];
    memcpy(compactSig, r.data(), 32);
    memcpy(compactSig + 32, s.data(), 32);

    // Try recid from 0..3
    for (int recid = 0; recid < 4; recid++) {
        if (secp256k1_ecdsa_recoverable_signature_parse_compact(ctx, &sig, compactSig, recid)) {
            secp256k1_pubkey pubkey;
            if (secp256k1_ecdsa_recover(ctx, &pubkey, &sig, hash.data())) {
                // Serialize recovered pubkey (uncompressed 65 bytes)
                size_t outlen = 65;
                secp256k1_ec_pubkey_serialize(ctx, outPubkey.data(), &outlen, &pubkey, SECP256K1_EC_UNCOMPRESSED);
                secp256k1_context_destroy(ctx);
                return recid;
            }
        }
    }

    secp256k1_context_destroy(ctx);
    return -1; // not found
}
