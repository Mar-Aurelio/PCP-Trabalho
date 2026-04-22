#pragma once

#include <cstdint>

class SFC64 {
  private:
    uint64_t a, b, c, counter;

  public:
    SFC64(uint64_t seed) {
      a = seed;
      b = seed ^ 0xdeadbeefcafebabe;
      c = (seed << 32) | (seed >> 32);
      counter = 1;

      Next();
    }

    uint64_t Next() {
      uint64_t result = a + b + counter++;
      a = b ^ (b >> 11);
      b = c + (c << 3);
      c = ((c << 24) | (c >> 40)) + result;
      return result;
    }

    int64_t NextInt(int64_t min, int64_t max) {
      uint64_t range = (uint64_t)(max - min);
      __uint128_t m = (__uint128_t)Next() * range;
      uint64_t l = (uint64_t)m;  // low 64 bits
      if (l < range) {
        uint64_t threshold = (-range) % range;
        while (l < threshold) {
          m = (__uint128_t)Next() * range;
          l = (uint64_t)m;
        }
      }

      return (int64_t)((uint64_t)(m >> 64) + (uint64_t)min);
    }
};
