#pragma once

#include <string>
#include <vector>
#include <array>
#include <cstdint>

class ValidatorRegistry {
public:
    // Load validator list (from config or on‑chain)
    static void loadValidators();

    // Check if an address is an authorized validator
    static bool isValidator(const std::array<uint8_t,20> &addr);

    // Return all current validator addresses
    static const std::vector<std::array<uint8_t,20>>& getValidators();

private:
    static std::vector<std::array<uint8_t,20>> validators;
};
