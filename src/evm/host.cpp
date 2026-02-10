#include "host.h"
#include "storage.h"

#include <iostream>
#include <algorithm>
#include <cstring>

// ==========================
// Constructor
// ==========================

MedorEVMHost::MedorEVMHost(EVMStorage &storage)
    : storageDB(storage)
{
}

// ==========================
// Persistent Storage Access
// ==========================

evmc_bytes32 MedorEVMHost::get_storage(evmc::address addr, evmc_bytes32 key)
{
    // Convert address + key bytes to strings
    std::string addrStr(reinterpret_cast<const char*>(addr.bytes), sizeof(addr.bytes));
    std::string keyStr(reinterpret_cast<const char*>(key.bytes), sizeof(key.bytes));

    // Read from RocksDB
    std::string stored = storageDB.getContractStorage(addrStr, keyStr);

    evmc_bytes32 out{};
    if (!stored.empty()) {
        // Copy up to 32 bytes
        std::memcpy(out.bytes, stored.data(), std::min(stored.size(), sizeof(out.bytes)));
    }
    return out;
}

evmc_storage_status MedorEVMHost::set_storage(
    evmc::address addr,
    evmc_bytes32 key,
    evmc_bytes32 val)
{
    // Convert address + key + value to strings
    std::string addrStr(reinterpret_cast<const char*>(addr.bytes), sizeof(addr.bytes));
    std::string keyStr(reinterpret_cast<const char*>(key.bytes), sizeof(key.bytes));

    std::string valStr(reinterpret_cast<const char*>(val.bytes), sizeof(val.bytes));

    // Write into RocksDB
    bool ok = storageDB.putContractStorage(addrStr, keyStr, valStr);

    return ok ? EVMC_STORAGE_MODIFIED : EVMC_STORAGE_UNCHANGED;
}

// ==========================
// Contract Code Access
// ==========================

evmc_bytes32 MedorEVMHost::get_code_hash(evmc::address addr)
{
    // Get bytecode from persistent storage
    std::vector<uint8_t> code = storageDB.getContractCode(
        std::string(reinterpret_cast<const char*>(addr.bytes), sizeof(addr.bytes)));

    evmc_bytes32 hash{};
    if (code.empty()) return hash;

    // Simple placeholder — later replace with a real hash (Keccak256)
    for (size_t i = 0; i < code.size() && i < sizeof(hash.bytes); ++i) {
        hash.bytes[i] = code[i];
    }
    return hash;
}

std::vector<uint8_t> MedorEVMHost::get_code(evmc::address addr)
{
    return storageDB.getContractCode(
        std::string(reinterpret_cast<const char*>(addr.bytes), sizeof(addr.bytes)));
}

// ==========================
// Balance Handling
// ==========================

uint64_t MedorEVMHost::get_balance(evmc::address addr)
{
    // Default 0 until native balance support is added
    return storageDB.getBalance(
        std::string(reinterpret_cast<const char*>(addr.bytes), sizeof(addr.bytes)));
}

// ==========================
// Event Logging
// ==========================

void MedorEVMHost::emit_log(
    evmc::address addr,
    const uint8_t *data,
    size_t data_size,
    const evmc::bytes32 topics[],
    size_t topics_count)
{
    std::cout << "[EVM LOG] Contract: 0x";
    for (auto b : addr.bytes) std::cout << std::hex << (int)b;
    std::cout << " DataSize: " << std::dec << data_size << std::endl;
}
