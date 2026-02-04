#include "wallet.h"
#include "utxo.h"
#include <iostream>

Wallet::Wallet() {
    privateKey = generatePrivateKey();
    publicKey = getPublicKey(privateKey);
    address = doubleSHA256(publicKey); // Simple address derivation
}

Transaction Wallet::createTransaction(const std::string& recipient, uint64_t amount) {
    std::vector<TxInput> inputs;
    std::vector<TxOutput> outputs;

    uint64_t accumulated = 0;
    for (auto &utxo : UTXOSet::getInstance().getUTXOs(address)) {
        TxInput input;
        input.prevTxHash = utxo.txid;
        input.outputIndex = utxo.index;
        input.signature = signMessage(utxo.txid, privateKey);
        inputs.push_back(input);
        accumulated += utxo.value;
        if (accumulated >= amount) break;
    }

    if (accumulated < amount) {
        std::cerr << "Not enough balance!" << std::endl;
        return Transaction(); // Empty transaction
    }

    outputs.push_back({amount, recipient});
    if (accumulated > amount) {
        outputs.push_back({accumulated - amount, address}); // Change back
    }

    return Transaction(inputs, outputs);
}
