#pragma once

#include <cstdint>

class SFC64 {
  private:
    uint64_t a, b, c, counter;

    uint64_t Next() {
      uint64_t result = a + b + counter++;
      a = b ^ (b >> 11);
      b = c + (c << 3);
      c = ((c << 24) | (c >> 40)) + result;
      return result;
    }

  public:
    SFC64(uint64_t seed) {
      a = seed;
      b = seed ^ 0xdeadbeefcafebabe;
      c = (seed << 32) | (seed >> 32);
      counter = 1;

      Next();
    }

    int64_t NextInt(int64_t min, int64_t max) {
      double sample = (double)Next() / (double)UINT64_MAX;
      uint64_t range = max - min;
      return (int64_t)(range * sample + min);
    }
};
