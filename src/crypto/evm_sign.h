#pragma once

#include <vector>
#include <string>
#include "../transaction.h"

std::vector<uint8_t> signEvmTransaction(
    Transaction &tx,
    const std::string &privKeyPath
);
