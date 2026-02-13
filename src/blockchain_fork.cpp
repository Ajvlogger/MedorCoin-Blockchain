#include "blockchain.h"
#include <algorithm>
#include <iostream>

/**
 * Fork resolution: longest valid chain wins.
 */
bool Blockchain::resolveFork(const std::vector<Block> &candidateChain) {
    // Must start with the same genesis block
    if (candidateChain.empty() || chain.empty()) return false;
    if (candidateChain.front().hash != chain.front().hash) return false;

    // Compare lengths; longer chain wins
    if (candidateChain.size() > chain.size()) {
        // Validate entire candidate chain before switching
        for (size_t i = 1; i < candidateChain.size(); ++i) {
            const Block &prev = candidateChain[i - 1];
            const Block &curr = candidateChain[i];

            // Check previousHash match
            if (curr.previousHash != prev.hash) {
                std::cerr << "[FORK] Invalid candidate block at index " << i << std::endl;
                return false;
            }

            // Optionally add other validation: signature, merkle, gas limits, etc.
        }

        // Replace local chain with the candidate chain
        chain = candidateChain;
        std::cout << "[FORK] Chain replaced with a longer fork of length "
                  << chain.size() << std::endl;
        return true;
    }

    return false;
}
