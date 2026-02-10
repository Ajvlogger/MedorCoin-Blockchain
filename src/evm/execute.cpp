#include "execute.h"
#include "host.h"
#include <evmone/evmone.h>
#include <iostream>

evmc_result EVMExecutor::executeContract(const std::vector<uint8_t>& bytecode,
                                         const std::vector<uint8_t>& inputData,
                                         uint64_t gasLimit,
                                         evmc_address to,
                                         evmc_address from)
{
    static auto instance = evmc::VM{evmc_create_evmone()};

    MedorEVMHost host;

    evmc_message msg{};
    msg.kind = EVMC_CALL;
    msg.depth = 0;
    msg.gas = gasLimit;
    msg.destination = to;
    msg.sender = from;
    msg.input_data = inputData.data();
    msg.input_size = inputData.size();
    msg.value = 0;

    evmc_result result = instance.execute(
        /* host */ host,
        /* revision */ EVMC_CANCUN,
        /* code */ bytecode.data(),
        /* code_size */ bytecode.size(),
        /* msg */ &msg
    );

    if (result.status_code != EVMC_SUCCESS) {
        std::cout << "EVM execution failed, status: " << result.status_code << std::endl;
    }
    return result;
}
