#ifndef NAT_H__
#define NAT_H__

#include <cstdint>

using Nat = std::size_t;

constexpr Nat log2(Nat n) {
  Nat out = 0;
  while (n > 1) {
    ++out;
    n >>= 1;
  }
  return out;
}

constexpr bool is_pow2(Nat n) {
  if (n == 0) { return false; }
  return (1 << log2(n)) == n;
}

constexpr Nat ceil_div(Nat x, Nat y) {
  return (x + y - 1) / y;
}

constexpr bool divides(Nat x, Nat y) {
  if (y == 0) { return true; }
  else { return y % x == 0; }
}

constexpr Nat natpow(Nat x, Nat p) {
  if (p == 0) { return 1; }
  if (p == 1) { return x; }

  const Nat xp2 = natpow(x, p/2);
  if (p % 2 == 0) {
    return xp2 * xp2;
  } else {
    return x * xp2 * xp2;
  }
}

#endif
