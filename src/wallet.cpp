#include "wallet.h"
#include <openssl/ec.h>
#include <openssl/obj_mac.h>
#include <openssl/rand.h>
#include <stdexcept>

Wallet::Wallet() {}

void Wallet::generateKeys() {
    EC_KEY* ecKey = EC_KEY_new_by_curve_name(NID_secp256k1);
    if (!EC_KEY_generate_key(ecKey)) {
        throw std::runtime_error("Failed to generate EC key");
    }

    const BIGNUM* priv = EC_KEY_get0_private_key(ecKey);
    privateKey.resize(32); // 256-bit key
    BN_bn2bin(priv, privateKey.data());

    int pubLen = i2o_ECPublicKey(ecKey, nullptr);
    publicKey.resize(pubLen);
    unsigned char* pubPtr = publicKey.data();
    i2o_ECPublicKey(ecKey, &pubPtr);

    EC_KEY_free(ecKey);
}
