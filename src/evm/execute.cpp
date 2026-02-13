#include "execute.h"
#include "host.h"
#include "storage.h"
#include "blockchain.h"
#include <evmone/evmone.h>
#include <evmc/evmc.hpp>
#include <iostream>
#include <algorithm>

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
    // 1) Run EVM using evmone
    static evmc::VM vm{evmc_create_evmone()};

    MedorEVMHost host(storage, chain);

    evmc_message msg{};
    msg.kind = EVMC_CALL;
    msg.flags = 0;
    msg.depth = 0;
    msg.gas = gasLimit;
    msg.destination = to;
    msg.sender = from;
    msg.input_data = inputData.data();
    msg.input_size = inputData.size();
    msg.value = {}; // no native value in this example

    evmc_result result = vm.execute(
        host,
        EVMC_CANCUN,
        msg,
        bytecode.data(),
        bytecode.size()
    );

    // 2) Calculate gas used
    uint64_t gasUsed = gasLimit - result.gas_left;

    // 3) Fee market (EIP‑1559 style)
    uint64_t baseFee      = chain.getCurrentBaseFee();
    uint64_t tip          = chain.getSuggestedPriority(tx);   // priority inside tx
    uint64_t maxFee       = chain.getTxMaxFee(tx);            // user max

    uint64_t effectivePrice = std::min(maxFee, baseFee + tip);
    uint64_t totalFee       = gasUsed * effectivePrice;
    uint64_t baseFeeTotal   = gasUsed * baseFee;
    uint64_t priorityTotal  = gasUsed * tip;

    // 4) Deduct fees from sender
    uint64_t senderBal = chain.getBalance(tx.fromAddress);
    if (senderBal < totalFee) {
        std::cerr << "[EVM] Insufficient balance for fees\n";
        return result;
    }
    chain.setBalance(tx.fromAddress, senderBal - totalFee);

    // 5) Pay miner priority tip
    uint64_t minerBal = chain.getBalance(minerAddress);
    chain.setBalance(minerAddress, minerBal + priorityTotal);

    // 6) Burn base fee to treasury
    chain.burnBaseFees(baseFeeTotal);

    // 7) Update gas used in current block
    chain.currentBlock.gasUsed += gasUsed;

    return result;
}
