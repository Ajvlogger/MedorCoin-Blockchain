#include "bloom_utils.h"
#include <openssl/sha.h> // or your keccak

// Example simple bit selection (replace with proper keccak256→3 bits)
void updateBloom(std::array<uint8_t,256> &b, const std::vector<uint8_t> &data) {
    uint8_t hash[32];
    keccak(data.data(), data.size(), hash, 32);
    for (int i = 0; i < 3; i++) {
        uint16_t bit = ((hash[i*2] << 8) | hash[i*2+1]) & 2047;
        b[bit >> 3] |= (1 << (bit & 7));
    }
}

void updateBloom(std::array<uint8_t,256> &b, const std::array<uint8_t,20> &addr) {
    std::vector<uint8_t> d(addr.begin(), addr.end());
    updateBloom(b, d);
}

void updateBloom(std::array<uint8_t,256> &b, const std::array<uint8_t,32> &topic) {
    std::vector<uint8_t> d(topic.begin(), topic.end());
    updateBloom(b, d);
}
