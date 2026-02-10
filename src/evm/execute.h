#ifndef MEDOR_EVM_EXECUTE_H
#define MEDOR_EVM_EXECUTE_H

#include <vector>
#include <string>
#include <evmc/evmc.hpp>

class EVMExecutor
{
public:
    static evmc_result executeContract(const std::vector<uint8_t>& bytecode,
                                       const std::vector<uint8_t>& inputData,
                                       uint64_t gasLimit,
                                       evmc_address to,
                                       evmc_address from);
};

#endif // MEDOR_EVM_EXECUTE_H
