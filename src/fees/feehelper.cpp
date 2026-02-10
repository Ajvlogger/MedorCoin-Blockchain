#include "feehelper.h"

uint64_t FeeHelper::recommendedBaseFee(uint64_t lastBaseFee) {
    return lastBaseFee + lastBaseFee / 10; // +10%
}

uint64_t FeeHelper::recommendedPriority(uint64_t recentTipAvg) {
    return recentTipAvg + (recentTipAvg / 5); // +20%
}
