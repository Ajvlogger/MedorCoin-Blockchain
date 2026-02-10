#include "execute.h"
#include "host.h"
#include <evmone/evmone.hpp>
#include <iostream>
#include <algorithm>

// Executes EVM code and returns result + gas used
EVMExecutionResult EVMExecutor::executeContract(
    EVMStorage &storage,
    const std::vector<uint8_t> &bytecode,
    const std::vector<uint8_t> &inputData,
    uint64_t gasLimit,
    evmc_address to,
    evmc_address from
) {
    // Initialize VM (evmone)
    static auto vm = evmc::VM{evmc_create_evmone()};

    // Create host backed by RocksDB
    MedorEVMHost host(storage);

    // Prepare message
    evmc_message msg{};
    msg.kind = EVMC_CALL;
    msg.depth = 0;
    msg.gas = gasLimit;
    msg.destination = to;
    msg.sender = from;
    msg.input_data = inputData.data();
    msg.input_size = inputData.size();
    msg.flags = 0;
    msg.value = 0; // native value (not yet supported)

    // Execute
    evmc_result result = vm.execute(
        /* host */ host,
        /* revision */ EVMC_CANCUN,
        /* code */ bytecode.data(),
        /* code_size */ bytecode.size(),
        /* msg */ &msg
    );

    // Compute gas used
    uint64_t gasLeft = result.gas_left;
    uint64_t gasUsed = (gasLimit > gasLeft ? gasLimit - gasLeft : 0);

    // Prepare output
    std::vector<uint8_t> out;
    if (result.output_size > 0 && result.output_data != nullptr) {
        out.assign(result.output_data,
                   result.output_data + result.output_size);
    }

    // Show status
    if (result.status_code != EVMC_SUCCESS) {
        std::cout << "EVM execution failed, status: "
                  << result.status_code << std::endl;
    }

    // Return
    return EVMExecutionResult {
        result.status_code == EVMC_SUCCESS,
        gasUsed,
        out
    };
}
