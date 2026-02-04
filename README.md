MedorCoin Blockchain

MedorCoin is a Proof-of-Work blockchain implementing mining, genesis block creation, transaction verification, and a modular peer-to-peer architecture. It is inspired by Bitcoin but uses Medor as the symbol for difficulty (medor instead of bits).

⸻

Table of Contents
	1.	Overview￼
	2.	Features￼
	3.	Build Instructions￼
	4.	Quick Start￼
	5.	Project Structure￼
	6.	Contributing￼
	7.	License￼

⸻

Overview

MedorCoin is a lightweight, educational blockchain written in C++. It demonstrates the key components of a cryptocurrency:
	•	Genesis block creation
	•	Proof-of-Work mining
	•	Transaction creation and verification
	•	Wallet generation and key management
	•	UTXO tracking
	•	Merkle tree computation
	•	Basic peer-to-peer networking simulation

MedorCoin is not intended for production use. It is ideal for learning blockchain concepts or building prototype applications.

⸻

Features
	•	Blocks and Blockchain: Each block contains a Merkle root of transactions and links to the previous block.
	•	Proof-of-Work Mining: Blocks are mined using medor difficulty.
	•	Transactions: Supports inputs, outputs, and simplified digital signatures.
	•	Wallets: Generate private/public keys and addresses.
	•	UTXO Tracking: Keeps track of unspent transaction outputs.
	•	Mempool: Stores pending transactions before mining.
	•	Consensus Validation: Verifies blocks and full blockchain integrity.
	•	Networking: Simulates peer-to-peer block and transaction propagation.

⸻

Build Instructions

MedorCoin requires C++17 and OpenSSL for SHA256 hashing.

On Linux / macOS
	1.	Install dependencies:
  sudo apt update
sudo apt install build-essential libssl-dev cmake git
2.	Clone the repository:
git clone https://github.com/yourusername/MedorCoin-Blockchain.git
cd MedorCoin-Blockchain/src
3.	Compile the project:
g++ -std=c++17 *.cpp -lssl -lcrypto -o medorcoin
4.	Run MedorCoin:
./medorcoin


Quick Start

Here’s how to get started with MedorCoin after building it.

1. Create wallets

When you run ./medorcoin, wallets are automatically generated in main.cpp. For example:
Wallet alice;
Wallet bob;
std::cout << "Alice address: " << alice.address << std::endl;
std::cout << "Bob address: " << bob.address << std::endl;

2. Send MedorCoin

Alice can create a transaction to send MedorCoin to Bob:

Transaction tx = alice.createTransaction(bob.address, 50);
Mempool::getInstance().addTransaction(tx);
The transaction will now be stored in the mempool and included in the next block to be mined.

3. Mine pending transactions

The miner collects pending transactions from the mempool and mines a new block:
Miner miner(medorChain);
miner.minePendingTransactions();

After mining, the mempool is cleared and the new block is added to the chain.

4. Validate the blockchain

To ensure the blockchain is intact:
if (Consensus::validateChain(medorChain)) {
    std::cout << "Blockchain is valid!" << std::endl;
} else {
    std::cout << "Blockchain is invalid!" << std::endl;
}

Project Structure
src/
 ├─ block.h / block.cpp            # Block structure and mining
 ├─ blockchain.h / blockchain.cpp  # Blockchain logic
 ├─ transaction.h / transaction.cpp # Transactions, inputs, outputs
 ├─ wallet.h / wallet.cpp          # Wallet generation and transaction signing
 ├─ utxo.h / utxo.cpp              # UTXO tracking
 ├─ mempool.h / mempool.cpp        # Pending transaction storage
 ├─ miner.h / miner.cpp            # Mining pending transactions
 ├─ merkle.h / merkle.cpp          # Merkle tree computation
 ├─ crypto.h / crypto.cpp          # Hashing and simplified cryptography
 ├─ consensus.h / consensus.cpp    # Block and chain validation
 ├─ network.h / network.cpp        # Peer-to-peer simulation
 ├─ config.h                       # MedorCoin constants (name, symbol, difficulty)
 └─ main.cpp                        # Entry point for MedorCoin



 Contributing

Contributions are welcome! Please fork the repository, make your changes, and submit a pull request.

Areas for improvement:
	•	Real peer-to-peer networking over TCP/IP
	•	Persistent blockchain storage
	•	Transaction fees and prioritization
	•	Advanced consensus rules


License

This project is licensed under the MIT License. See LICENSE￼ for details.

⸻

This README is now complete and ready to go in README.md, with everything in one file: overview, features, build instructions, Quick Start examples, project structure, contributing guide, and license.









