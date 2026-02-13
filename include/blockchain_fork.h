#pragma once

#include <vector>
#include "block.h"

/**
 * Extends the core Blockchain class with fork resolution.
 * Declared in blockchain_fork.h, defined in blockchain_fork.cpp.
 */
class Blockchain;

bool Blockchain::resolveFork(const std::vector<Block> &candidateChain);
