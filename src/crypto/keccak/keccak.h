#pragma once
#include <stdint.h>
#include <stddef.h>

// outlen in bytes (32 for Keccak‑256)
void keccak(const uint8_t *in, size_t inlen, uint8_t *out, size_t outlen);

// Internal f‑function
void keccakf(uint64_t st[25]);
