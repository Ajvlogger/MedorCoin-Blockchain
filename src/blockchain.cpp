#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include "block.h"
#include "crypto.h"

using namespace std;

class Blockchain {
public:
    vector<Block> chain;
    uint32_t medor;

    // Constructor — set difficulty and add genesis block
    Blockchain() {
        medor = 0x1e00ffff; // Bitcoin-equivalent difficulty
        chain.push_back(createGenesisBlock());
    }

    // Create the first block (genesis)
    Block createGenesisBlock() {
        Block genesis("0", "Genesis Block", medor);   // make the block
        genesis.timestamp = time(nullptr);           // set timestamp
        genesis.hash = doubleSHA256(genesis.headerToString()); // compute hash
        return genesis;                              // return it
    }

    // Add a new block with given data
    void addBlock(const string& data) {
        Block newBlock(chain.back().hash, data, medor);
        newBlock.timestamp = time(nullptr);
        newBlock.hash = doubleSHA256(newBlock.headerToString());
        chain.push_back(newBlock);
    }

    // Optional: print the blockchain
    void printChain() const {
        for (size_t i = 0; i < chain.size(); i++) {
            cout << "Block " << i << ":\n";
            cout << "  Previous Hash: " << chain[i].previousHash << "\n";
            cout << "  Data: " << chain[i].data << "\n";
            cout << "  Hash: " << chain[i].hash << "\n";
            cout << "  Timestamp: " << chain[i].timestamp << "\n\n";
        }
    }
};

/* ---------- MAIN ---------- */
int main() {
    Blockchain medorCoin;

    medorCoin.addBlock("First MedorCoin block");
    medorCoin.addBlock("Second MedorCoin block");

    medorCoin.printChain(); // see the blockchain on console

    return 0;
}
