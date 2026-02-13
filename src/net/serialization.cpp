#include "serialization.h"

nlohmann::json serializeBlock(const Block &block) {
    nlohmann::json j;
    j["previousHash"] = block.previousHash;
    j["timestamp"] = block.timestamp;
    j["hash"] = block.hash;
    j["signature"] = block.signature;
    // Add transactions
    j["txs"] = nlohmann::json::array();
    for (auto &tx : block.transactions) {
        j["txs"].push_back(serializeTx(tx));
    }
    return j;
}

Block deserializeBlock(const nlohmann::json &j) {
    Block block;
    block.previousHash = j["previousHash"];
    block.timestamp = j["timestamp"];
    block.hash = j["hash"];
    block.signature = j["signature"];
    for (auto &t : j["txs"]) {
        block.transactions.push_back(deserializeTx(t));
    }
    return block;
}

nlohmann::json serializeTx(const Transaction &tx) {
    nlohmann::json j;
    j["txHash"] = tx.txHash;
    j["fromAddress"] = tx.fromAddress;
    j["toAddress"] = tx.toAddress;
    j["gasLimit"] = tx.gasLimit;
    j["maxFee"] = tx.maxFeePerGas;
    j["priority"] = tx.maxPriorityFeePerGas;
    j["data"] = nlohmann::json::binary(tx.data);
    return j;
}

Transaction deserializeTx(const nlohmann::json &j) {
    Transaction tx;
    tx.txHash = j["txHash"];
    tx.fromAddress = j["fromAddress"];
    tx.toAddress = j["toAddress"];
    tx.gasLimit = j["gasLimit"];
    tx.maxFeePerGas = j["maxFee"];
    tx.maxPriorityFeePerGas = j["priority"];
    tx.data = j["data"].get<std::vector<uint8_t>>();
    return tx;
}
