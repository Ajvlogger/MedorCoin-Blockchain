#include "wallet.h"
#include <openssl/ec.h>
#include <openssl/obj_mac.h>
#include <openssl/sha.h>
#include <openssl/ripemd.h>
#include <stdexcept>
#include <vector>
#include <string>

// Base58 alphabet
static const char* BASE58_ALPHABET = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";

// Compute SHA256
static std::vector<unsigned char> sha256(const std::vector<unsigned char>& data) {
    std::vector<unsigned char> hash(SHA256_DIGEST_LENGTH);
    SHA256(data.data(), data.size(), hash.data());
    return hash;
}

// Compute RIPEMD160
static std::vector<unsigned char> ripemd160(const std::vector<unsigned char>& data) {
    std::vector<unsigned char> hash(RIPEMD160_DIGEST_LENGTH);
    RIPEMD160(data.data(), data.size(), hash.data());
    return hash;
}

// Encode bytes to Base58Check
static std::string encodeBase58Check(const std::vector<unsigned char>& data) {
    // Convert to big integer
    std::vector<unsigned char> input(data.begin(), data.end());

    // Count leading zeros
    int zeros = 0;
    for (unsigned char c : input) {
        if (c == 0) zeros++;
        else break;
    }

    // Convert to base58
    std::vector<unsigned long long> digits;
    digits.push_back(0);

    for (unsigned char byte : input) {
        unsigned long long carry = byte;
        for (size_t j = 0; j < digits.size(); j++) {
            carry += digits[j] << 8;
            digits[j] = carry % 58ULL;
            carry /= 58ULL;
        }
        while (carry) {
            digits.push_back(carry % 58ULL);
            carry /= 58ULL;
        }
    }

    // Leading '1's
    std::string result;
    for (int i = 0; i < zeros; i++)
        result += '1';

    // Convert digits
    for (auto it = digits.rbegin(); it != digits.rend(); ++it)
        result += BASE58_ALPHABET[*it];

    return result;
}

Wallet::Wallet() {}

// Generate secp256k1 private/public key
void Wallet::generateKeys() {
    EC_KEY* ecKey = EC_KEY_new_by_curve_name(NID_secp256k1);
    if (!EC_KEY_generate_key(ecKey)) {
        throw std::runtime_error("Failed to generate EC key");
    }

    const BIGNUM* priv = EC_KEY_get0_private_key(ecKey);
    privateKey.resize(32);
    BN_bn2bin(priv, privateKey.data());

    int pubLen = i2o_ECPublicKey(ecKey, nullptr);
    publicKey.resize(pubLen);
    unsigned char* pubPtr = publicKey.data();
    i2o_ECPublicKey(ecKey, &pubPtr);

    EC_KEY_free(ecKey);
}

// Bitcoin‑style Base58 address generation
void Wallet::generateAddress() {
    if (publicKey.empty())
        throw std::runtime_error("No public key generated");

    // 1. SHA256 then RIPEMD160 (hash160)
    std::vector<unsigned char> sha = sha256(publicKey);
    std::vector<unsigned char> rip = ripemd160(sha);

    // 2. Prepend version byte
    std::vector<unsigned char> versioned;
    versioned.push_back(0x00); // legacy prefix (Bitcoin uses 0x00)  [oai_citation:1‡en.bitcoin.it](https://en.bitcoin.it/wiki/Base58Check_encoding?utm_source=chatgpt.com)
    versioned.insert(versioned.end(), rip.begin(), rip.end());

    // 3. Compute checksum
    auto hash1 = sha256(versioned);
    auto hash2 = sha256(hash1);
    versioned.insert(versioned.end(), hash2.begin(), hash2.begin()+4);

    // 4. Base58Check encode
    address = encodeBase58Check(versioned);
}
