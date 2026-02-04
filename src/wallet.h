#pragma once
#include <string>

class Wallet {
public:
    std::string privateKey;
    std::string publicKey;
    std::string address;

    Wallet(); // Generate keys
};
