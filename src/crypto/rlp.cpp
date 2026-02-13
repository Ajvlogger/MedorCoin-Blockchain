#include "rlp.h"
#include <algorithm>

namespace rlp {

static std::vector<uint8_t> encodeLength(size_t len, uint8_t offset) {
    std::vector<uint8_t> out;
    if (len < 56) {
        out.push_back(uint8_t(offset + len));
    } else {
        // big‑endian
        std::vector<uint8_t> tmp;
        while (len > 0) {
            tmp.push_back(uint8_t(len & 0xff));
            len >>= 8;
        }
        std::reverse(tmp.begin(), tmp.end());
        out.push_back(uint8_t(offset + 55 + tmp.size()));
        out.insert(out.end(), tmp.begin(), tmp.end());
    }
    return out;
}

std::vector<uint8_t> encodeBytes(const std::vector<uint8_t>& input) {
    if (input.size() == 1 && input[0] < 0x80) {
        return input; // single byte < 0x80
    }
    auto prefix = encodeLength(input.size(), 0x80);
    std::vector<uint8_t> out = prefix;
    out.insert(out.end(), input.begin(), input.end());
    return out;
}

std::vector<uint8_t> encodeUInt(uint64_t value) {
    if (value == 0) return {0x80};
    std::vector<uint8_t> tmp;
    while (value > 0) {
        tmp.push_back(uint8_t(value & 0xff));
        value >>= 8;
    }
    std::reverse(tmp.begin(), tmp.end());
    return encodeBytes(tmp);
}

std::vector<uint8_t> encodeList(const std::vector<std::vector<uint8_t>>& items) {
    std::vector<uint8_t> payload;
    for (auto &it : items) {
        payload.insert(payload.end(), it.begin(), it.end());
    }
    auto prefix = encodeLength(payload.size(), 0xc0);
    std::vector<uint8_t> out = prefix;
    out.insert(out.end(), payload.begin(), payload.end());
    return out;
}

} // namespace rlp
