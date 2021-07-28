#ifndef UNARY_H__
#define UNARY_H__


#include "nat.h"

#include <vector>


// The overall goal is to construct a mapping from binary numbers to
// unary numbers via a Boolean circuit.
// We wish to do so using the lowest possible multiplicative complexity.
//
// The first step is to compute, not unary, but rather a "full expansion" of
// all combinations of variable products.
//
// For example, given variables a and b, we wish to construct the following
// products:
//
// 0: 1
// 1: a
// 2: ~a
// 3: b
// 4: ab
// 5: ~ab
// 6: ~b
// 7: a~b
// 8: ~a~b
//
// That is, we wish to populate a table where each variable can independently
// (1) not appear, (2) appear in positive position, or (3) appear in negative
// position.
// This table can be iteratively constructed starting from index 0.
// For a table with `n` variables, `2^(n-1)` multiplications are required.
// The remaining rows are computed by linear operations.
// With the full expansion calculated, the unary expansion of a binary number
// just selects particular rows of this table.

// Because each variable can take on three states, we need a ternary logic
using Trit = std::uint8_t;


void decompose_trits(Nat inp, std::span<Trit> out) {
  for (Nat j = out.size(); j > 0; --j) {
    const Nat p = natpow(3, j-1);
    out[j-1] = inp/p;
    assert(out[j-1] <= 2);
    inp = inp % p;
  }
};


Nat compose_trits(std::span<const Trit> inp) {
  Nat p = 1;
  Nat out = 0;
  for (auto i : inp) {
    out += i*p;
    p *= 3;
  }
  return out;
}


bool all_zero_one(std::span<const Trit> inp) {
  for (auto i: inp) {
    if (i > 1) { return false; }
  }
  return true;
}


bool singleton(std::span<const Trit> inp, Nat& where) {
  bool single = false;
  for (Nat i = 0; i < inp.size(); ++i) {
    if (inp[i] > 0 && !single) {
      single = true;
      where = i;
    } else if (inp[i] > 0) {
      where = 0;
      return false;
    }
  }
  return single;
}


std::pair<Nat, Nat> split_mst(Nat n) {
  Nat p = 1;
  while (p <= n) { p *= 3; }

  p /= 3;
  const Nat top = (n/p) * p;
  const Nat rest = n - top;

  return { top, rest };
}


Nat top_two(std::span<const Trit> ts) {
  Nat out = -1;
  for (Nat i = 0; i < ts.size(); ++i) {
    if (ts[i] == 2) { out = i; }
  }
  return out;
}


template <typename Bool>
void expansions(std::span<const Bool> inp, std::span<Bool> out) {
  const Nat n = inp.size();
  const Nat m = natpow(3, n);

  assert(out.size() == m);

  std::vector<Trit> trits(n);
  out[0] = Bool::constant(true);
  for (Nat i = 1; i < m; ++i) {
    decompose_trits(i, trits);

    Nat where = 0;
    if (all_zero_one(trits)) {
      if (singleton(trits, where)) {
        out[i] = inp[where];
      } else {
        const auto [t, r] = split_mst(i);
        out[i] = out[t] & out[r];
      }
    } else {
      const auto tt = natpow(3, top_two(trits));
      out[i] = out[i - tt] ^ out[i - 2*tt];
    }
  }
}


template <typename Bool>
void unary(std::span<const Bool> inp, std::span<Bool> out) {
  const Nat n = inp.size();
  const Nat m = 1 << n;
  assert(out.size() == m);

  std::vector<Bool> exps(natpow(3, inp.size()));
  expansions<Bool>(inp, exps);

  std::vector<Trit> trits(n);
  for (Nat i = 0; i < m; ++i) {
    for (Nat j = 0; j < n; ++j) {
      trits[j] = 2 - ((i & (1 << j)) > 0);
    }
    out[i] = exps[compose_trits(trits)];
  }
}


template <typename Bool>
std::vector<Bool> unary(std::span<const Bool> inp) {
  std::vector<Bool> u(1 << inp.size());
  unary<Bool>(inp, u);
  return u;
}


#endif
