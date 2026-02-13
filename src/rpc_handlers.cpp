#include "rpc_handlers.h"
#include "blockchain.h"
#include <nlohmann/json.hpp>

// Helper to convert hex without 0x
static std::string toHex(const std::vector<uint8_t> &bytes) {
    std::ostringstream ss;
    ss << "0x";
    for (auto b : bytes) ss << std::hex << std::setw(2) << std::setfill('0') << (int)b;
    return ss.str();
}

std::string rpc_getTransactionReceipt(const nlohmann::json &params, int id) {
    nlohmann::json response;
    response["jsonrpc"] = "2.0";
    response["id"] = id;

    // Expect exactly one parameter: txHash
    if (!params.is_array() || params.size() != 1) {
        response["error"] = {{"code", -32602}, {"message", "Invalid params"}};
        return response.dump();
    }

    std::string txHash = params[0].get<std::string>();

    // Look up the receipt in your storage
    auto optReceipt = ReceiptStore::getReceiptByHash(txHash);
    if (!optReceipt) {
        // Return null as per spec if not found or pending
        response["result"] = nullptr;
        return response.dump();
    }

    const TransactionReceipt &r = *optReceipt;

    nlohmann::json receiptJson;
    receiptJson["transactionHash"]  = txHash;
    receiptJson["blockHash"]        = "0x" + r.blockHash;
    receiptJson["blockNumber"]      = "0x" + intToHex(r.blockNumber);
    receiptJson["transactionIndex"] = "0x" + intToHex(r.transactionIndex);
    receiptJson["from"]             = "0x" + toHex(r.from);
    receiptJson["to"]               = "0x" + toHex(r.to);
    receiptJson["gasUsed"]          = "0x" + intToHex(r.gasUsed);
    receiptJson["cumulativeGasUsed"]= "0x" + intToHex(r.cumulativeGasUsed);
    receiptJson["logsBloom"]        = "0x" + bytesToHex(r.logsBloom);

    // Add logs array
    receiptJson["logs"] = nlohmann::json::array();
    for (auto &log : r.logs) {
        nlohmann::json logJson;
        logJson["address"] = "0x" + toHex(log.address);
        // topics
        logJson["topics"] = nlohmann::json::array();
        for (auto &topic : log.topics) {
            logJson["topics"].push_back("0x" + bytesToHex(topic));
        }
        logJson["data"] = "0x" + bytesToHex(log.data);
        receiptJson["logs"].push_back(logJson);
    }

    receiptJson["status"] = r.status ? "0x1" : "0x0";

    response["result"] = receiptJson;
    return response.dump();
}
