#pragma once
#include <string>
#include <vector>

class Wallet {
public:
    Wallet();

    void generateKeys();                // Step 2.1 & 2.3
    std::string getAddress() const;

private:
    std::vector<unsigned char> privateKey;  // 256-bit key
    std::vector<unsigned char> publicKey;
    std::string address;
};
