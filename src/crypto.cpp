#include "crypto.h"
#include <openssl/sha.h>
#include <sstream>
#include <iomanip>
#include <random>
#include <iostream>

using namespace std;

string doubleSHA256(const string& input) {
    unsigned char hash1[SHA256_DIGEST_LENGTH];
    unsigned char hash2[SHA256_DIGEST_LENGTH];

    SHA256((unsigned char*)input.c_str(), input.size(), hash1);
    SHA256(hash1, SHA256_DIGEST_LENGTH, hash2);

    stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << hex << setw(2) << setfill('0') << (int)hash2[i];
    }
    return ss.str();
}

string generatePrivateKey() {
    random_device rd;
    mt19937_64 gen(rd());
    uniform_int_distribution<uint64_t> dis;
    stringstream ss;
    ss << hex << dis(gen) << dis(gen);
    return ss.str();
}

// Simplified: public key is SHA256 of private key
string getPublicKey(const string& privateKey) {
    return doubleSHA256(privateKey);
}

// Simplified signature: SHA256(message + privateKey)
string signMessage(const string& message, const string& privateKey) {
    return doubleSHA256(message + privateKey);
}

// Simplified verification: recalc signature using pubKey
bool verifySignature(const string& message, const string& signature, const string& pubKey) {
    string testSig = doubleSHA256(message + pubKey); // Note: simplified
    return testSig == signature;
}
