#pragma once
#include <string>
#include <cstdint>
#include "block.h"

// Proof-of-Work consensus engine for MedorCoin
class ProofOfWork {
public:
    ProofOfWork(uint32_t difficultyTarget);

    // Mine a block (find nonce satisfying the target)
    void mineBlock(Block& block);

    // Check if a block satisfies PoW
    bool validateBlock(const Block& block) const;

private:
    uint32_t difficulty; // Number of leading zeros in hash
    std::string targetString() const; // Returns string of zeros for PoW target
};
