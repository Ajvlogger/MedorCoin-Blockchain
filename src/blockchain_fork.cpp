#include "blockchain_fork.h"
#include "blockchain.h"
#include <iostream>

/**
 * Compares candidate chain and local chain.
 * If candidate is longer and fully valid (links correctly), adopt it.
 */
bool resolveForkChain(const std::vector<Block> &candidateChain,
                      std::vector<Block> &localChain) {

    // Genesis must match
    if (candidateChain.empty() || localChain.empty()) return false;
    if (candidateChain.front().hash != localChain.front().hash) return false;

    // Only replace if candidate is longer
    if (candidateChain.size() > localChain.size()) {
        // Validate entire candidate
        for (size_t i = 1; i < candidateChain.size(); ++i) {
            const Block &prev = candidateChain[i - 1];
            const Block &curr = candidateChain[i];

            // Must link: curr.previousHash == prev.hash
            if (curr.previousHash != prev.hash) {
                std::cerr << "[Fork] Invalid chain at index " << i << std::endl;
                return false;
            }
        }

        // Accept new chain
        localChain = candidateChain;
        std::cout << "[Fork] Chain resolved — adopted longer chain of length "
                  << localChain.size() << std::endl;
        return true;
    }

    return false;
}
