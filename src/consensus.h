#ifndef CONSENSUS_H
#define CONSENSUS_H

#include <cstdint> // This ensures that uint32_t and other fixed-width integers are defined

class Consensus {
public:
    bool validateBlock(const Block& block, const Block& previousBlock);
    bool validateChain(const Blockchain& chain);
};

#endif // CONSENSUS_H
