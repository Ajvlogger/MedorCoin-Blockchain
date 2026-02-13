#pragma once

#include <vector>
#include "block.h"

/**
 * Extend blockchain with fork resolution.
 * Uses longest valid chain rule: the chain with more blocks wins.
 */
class Blockchain;

bool resolveForkChain(const std::vector<Block> &candidateChain,
                      std::vector<Block> &localChain);
