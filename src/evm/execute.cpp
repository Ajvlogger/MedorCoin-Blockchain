#include "execute.h"
#include "host.h"
#include "storage.h"
#include "blockchain.h"  // for block data
#include <evmone/evmone.h>
#include <iostream>
#include <algorithm>

// Executes a contract and handles gas + fee accounting
evmc_result EVMExecutor::executeContract(
    EVMStorage &storage,
    const std::vector<uint8_t> &bytecode,
    const std::vector<uint8_t> &inputData,
    uint64_t gasLimit,
    evmc_address to,
    evmc_address from,
    Blockchain &chain,
    const std::string &minerAddress
) {
    // 1) Run EVM
    static auto vm = evmc::VM{evmc_create_evmone()};
    MedorEVMHost host(storage);

    evmc_message msg{};
    msg.kind = EVMC_CALL;
    msg.depth = 0;
    msg.gas = gasLimit;
    msg.destination = to;
    msg.sender = from;
    msg.input_data = inputData.data();
    msg.input_size = inputData.size();
    msg.flags = 0;
    msg.value = 0;

    evmc_result result = vm.execute(
        host,
        EVMC_CANCUN,
        bytecode.data(),
        bytecode.size(),
        &msg
    );

    // 2) Compute gas used
    uint64_t gasUsed = gasLimit - result.gas_left;

    // 3) Compute fees
    uint64_t baseFee   = chain.getCurrentBaseFee();          // from consensus
    uint64_t tip       = chain.getSuggestedPriority(tx);      // user priority
    uint64_t maxFee    = chain.getTxMaxFee(tx);               // user max cap

    uint64_t effectiveGasPrice = std::min(maxFee, baseFee + tip);
    uint64_t totalFee      = gasUsed * effectiveGasPrice;
    uint64_t baseFeeTotal  = gasUsed * baseFee;
    uint64_t priorityTotal = gasUsed * tip;

    // 4) Deduct balance from sender
    uint64_t senderBal = chain.getBalance(fromAddress);
    if (senderBal < totalFee) {
        std::cerr << "[EVM] Insufficient balance for fees" << std::endl;
        return result;
    }

    chain.setBalance(fromAddress, senderBal - totalFee);

    // 5) Pay miner
    uint64_t minerBal = chain.getBalance(minerAddress);
    chain.setBalance(minerAddress, minerBal + priorityTotal);

    // 6) Burn base fee or send to treasury
    chain.burnBaseFees(baseFeeTotal);

    // 7) Update block gas usage
    chain.currentBlock.gasUsed += gasUsed;

    return result;
}
