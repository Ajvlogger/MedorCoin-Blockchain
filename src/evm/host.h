#ifndef MEDOR_EVM_HOST_H
#define MEDOR_EVM_HOST_H

#include <evmc/evmc.hpp>
#include <string>
#include <vector>

// EVMC Host interface: MedorCoin must define how the EVM interacts with blockchain state.

class MedorEVMHost : public evmc::Host
{
public:
    // Called by the EVM to get the contract storage value.
    evmc_bytes32 get_storage(evmc::address addr, evmc_bytes32 key) override;

    // Called by the EVM to set the contract storage value.
    evmc_storage_status set_storage(evmc::address addr, evmc_bytes32 key, evmc_bytes32 val) override;

    // Get the code of a contract at an address
    evmc_bytes32 get_code_hash(evmc::address addr) override;
    std::vector<uint8_t> get_code(evmc::address addr) override;

    // Get balance from your chain state
    uint64_t get_balance(evmc::address addr) override;

    // Called to emit logs
    void emit_log(evmc::address addr, const uint8_t* data, size_t data_size,
                  const evmc::bytes32 topics[], size_t topics_count) override;

    // Other required EVMC Host methods can be implemented later
};

#endif // MEDOR_EVM_HOST_H
