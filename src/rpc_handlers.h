#pragma once
#include <string>
#include <nlohmann/json.hpp>
#include "receipt.h"

/**
 * Handle eth_getTransactionReceipt JSON‑RPC method.
 *
 * @param params JSON‑RPC params (expected: [txHashString])
 * @param id     JSON‑RPC request id
 * @return JSON string response with full receipt fields
 */
std::string rpc_getTransactionReceipt(const nlohmann::json &params, int id);
