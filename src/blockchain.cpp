#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <openssl/sha.h>

using namespace std;

/* ---------- DOUBLE SHA256 ---------- */
string doubleSHA256(const string& input) {
    unsigned char hash1[SHA256_DIGEST_LENGTH];
    unsigned char hash2[SHA256_DIGEST_LENGTH];

    SHA256((unsigned char*)input.c_str(), input.size(), hash1);
    SHA256(hash1, SHA256_DIGEST_LENGTH, hash2);

    stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << hex << setw(2) << setfill('0') << (int)hash2[i];
    }
    return ss.str();
}

/* ---------- MEDOR (BITCOIN nBits STYLE) ---------- */
string medorToTarget(uint32_t medor) {
    uint32_t exponent = medor >> 24;
    uint32_t mantissa = medor & 0xFFFFFF;

    stringstream ss;
    ss << hex << mantissa;
    string target = ss.str();

    int zeros = (exponent - 3) * 2;
    return string(zeros, '0') + target;
}

/* ---------- BLOCK HEADER ---------- */
class Block {
public:
    int version;
    string previousHash;
    string merkleRoot;
    uint32_t timestamp;
    uint32_t medor;   // renamed from bits
    uint32_t nonce;
    string hash;

    Block(string prevHash, string data, uint32_t medorVal) {
        version = 1;
        previousHash = prevHash;
        merkleRoot = doubleSHA256(data);
        timestamp = time(nullptr);
        medor = medorVal;
        nonce = 0;
        hash = mine();
    }

    string headerToString() {
        return to_string(version) +
               previousHash +
               merkleRoot +
               to_string(timestamp) +
               to_string(medor) +
               to_string(nonce);
    }

    string mine() {
        string target = medorToTarget(medor);

        while (true) {
            string h = doubleSHA256(headerToString());
            if (h.substr(0, target.size()) <= target) {
                cout << "Block mined: " << h << endl;
                return h;
            }
            nonce++;
        }
    }
};

/* ---------- BLOCKCHAIN ---------- */
class Blockchain {
public:
    vector<Block> chain;
    uint32_t medor;

    Blockchain() {
        medor = 0x1e00ffff; // Bitcoin-equivalent difficulty, MedorCoin name
        chain.push_back(createGenesisBlock());
    }

    Block createGenesisBlock() {
        return Block("0", "Genesis Block", medor);
    }

    void addBlock(string data) {
        chain.push_back(Block(chain.back().hash, data, medor));
    }
};

/* ---------- MAIN ---------- */
int main() {
    Blockchain medorCoin;

    medorCoin.addBlock("First MedorCoin block");
    medorCoin.addBlock("Second MedorCoin block");

    return 0;
}
