#pragma once

#include <vector>
#include "block.h"

/**
 * Implements longest‑chain fork resolution.
 * If candidateChain is strictly longer and valid,
 * replaces localChain with candidate.
 *
 * @param candidateChain  The peer’s offered chain
 * @param localChain      Your current chain
 * @return true if localChain was replaced
 */
bool resolveLongestChain(const std::vector<Block> &candidateChain,
                         std::vector<Block> &localChain);
