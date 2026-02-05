#pragma once
#include "block.h"
#include "blockchain.h"

class Consensus {
public:
    bool validateBlock(const Block& block, const Block& previousBlock);
    bool validateChain(const Blockchain& chain);
};
