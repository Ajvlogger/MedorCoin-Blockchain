#include "abi/encode.h"
#include <sstream>
#include <iomanip>

// pad data to 32 bytes
static std::vector<uint8_t> pad32(const std::vector<uint8_t> &data) {
    std::vector<uint8_t> out(32);
    size_t start = 32 - data.size();
    for (size_t i = 0; i < data.size(); ++i)
        out[start + i] = data[i];
    return out;
}

std::vector<uint8_t> ABIEncoder::encodeAddress(const std::string &addr) {
    std::vector<uint8_t> raw;
    raw.reserve(20);
    for (size_t i = 0; i < addr.size(); i += 2) {
        std::stringstream ss;
        ss << std::hex << addr.substr(i, 2);
        unsigned int byte;
        ss >> byte;
        raw.push_back((uint8_t)byte);
    }
    return pad32(raw);
}

std::vector<uint8_t> ABIEncoder::encodeUint256(const std::string &value) {
    std::vector<uint8_t> result(32);
    unsigned long long v = std::stoull(value);
    for (int i = 31; i >= 0; --i) {
        result[i] = (uint8_t)(v & 0xff);
        v >>= 8;
    }
    return result;
}

std::vector<uint8_t> ABIEncoder::encodeBool(bool v) {
    std::vector<uint8_t> result(32);
    result[31] = v ? 1 : 0;
    return result;
}

std::vector<uint8_t> ABIEncoder::encodeCall(
    const std::vector<uint8_t> &selector,
    const std::vector<std::vector<uint8_t>> &args)
{
    std::vector<uint8_t> out;
    out.insert(out.end(), selector.begin(), selector.end());

    for (auto &a : args) {
        out.insert(out.end(), a.begin(), a.end());
    }
    return out;
}
