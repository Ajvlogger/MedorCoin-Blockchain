#include "wallet.h"
#include "crypto.h"

Wallet::Wallet() {
    // Generate private key securely
    privateKey = generatePrivateKey();
    publicKey = derivePublicKey(privateKey);
    address = hashPublicKey(publicKey); // SHA256+RIPEMD160
}
