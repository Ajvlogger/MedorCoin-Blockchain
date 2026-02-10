#include "host.h"
#include <iostream>
#include <unordered_map>

// Simple in‑memory stubs (replace with real LevelDB state)
static std::unordered_map<std::string, std::vector<uint8_t>> contract_code_db;
static std::unordered_map<std::string, std::unordered_map<std::string, evmc_bytes32>> contract_storage_db;
static std::unordered_map<std::string, uint64_t> balance_db;

evmc_bytes32 MedorEVMHost::get_storage(evmc::address addr, evmc_bytes32 key)
{
    std::string address_str(reinterpret_cast<const char*>(addr.bytes), 20);
    std::string key_str(reinterpret_cast<const char*>(key.bytes), 32);

    auto& storage = contract_storage_db[address_str];
    if (storage.find(key_str) != storage.end()) {
        return storage[key_str];
    }
    return evmc_bytes32{};
}

evmc_storage_status MedorEVMHost::set_storage(evmc::address addr, evmc_bytes32 key, evmc_bytes32 val)
{
    std::string address_str(reinterpret_cast<const char*>(addr.bytes), 20);
    std::string key_str(reinterpret_cast<const char*>(key.bytes), 32);

    contract_storage_db[address_str][key_str] = val;
    return EVMC_STORAGE_MODIFIED;
}

evmc_bytes32 MedorEVMHost::get_code_hash(evmc::address addr)
{
    std::string address_str(reinterpret_cast<const char*>(addr.bytes), 20);
    if (contract_code_db.find(address_str) != contract_code_db.end()) {
        // Compute simple hash (not real Keccak)
        evmc_bytes32 h {};
        const auto &code = contract_code_db[address_str];
        for (size_t i = 0; i < code.size() && i < 32; ++i) {
            h.bytes[i] = code[i];
        }
        return h;
    }
    return evmc_bytes32{};
}

std::vector<uint8_t> MedorEVMHost::get_code(evmc::address addr)
{
    std::string address_str(reinterpret_cast<const char*>(addr.bytes), 20);
    if (contract_code_db.find(address_str) != contract_code_db.end()) {
        return contract_code_db[address_str];
    }
    return {};
}

uint64_t MedorEVMHost::get_balance(evmc::address addr)
{
    std::string address_str(reinterpret_cast<const char*>(addr.bytes), 20);
    if (balance_db.find(address_str) != balance_db.end()) {
        return balance_db[address_str];
    }
    return 0;
}

void MedorEVMHost::emit_log(evmc::address addr, const uint8_t* data, size_t data_size,
                            const evmc::bytes32 topics[], size_t topics_count)
{
    std::cout << "EVM Log emitted (not stored): " << std::string(reinterpret_cast<const char*>(data), data_size) << std::endl;
}
