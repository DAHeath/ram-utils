#ifndef PERMUTE_H__
#define PERMUTE_H__


#include "nat.h"

#include <span>
#include <vector>


struct BitPtr {
public:
  BitPtr() { }
  BitPtr(std::vector<bool>& data) : data(&data), offset(0) { }
  BitPtr(std::vector<bool>* data, Nat offset)
    : data(data), offset(offset) { }

  auto operator[](Nat ix) { return (*data)[ix + offset]; }
  auto operator[](Nat ix) const { return (*data)[ix + offset]; }

  BitPtr operator+(Nat off) const { return { data, offset + off }; }

private:
  std::vector<bool>* data;
  Nat offset;
};


void permute(
    Nat logn,
    Nat* src_to_tgt,
    Nat* tgt_to_src,
    bool visit_start,
    BitPtr visited,
    BitPtr buffer,
    Nat* xs) {

  if (logn == 1) {
    if (src_to_tgt[0] > src_to_tgt[1]) {
      std::swap(xs[0], xs[1]);
    }
  } else {
    const auto n4 = 1 << (logn - 2);
    const auto n2 = n4 << 1;
    const auto n = n2 << 1;

    for (Nat i = 0; i < n; ++i) { tgt_to_src[src_to_tgt[i]] = i; }

    const auto modn2 = [&] (std::size_t x) { return x >= n2 ? x - n2 : x; };
    const auto cong = [&] (std::size_t x) { return x >= n2 ? x - n2 : x + n2; };

    // Program the first layer of swaps
    // The key idea is to send each member of a congruent pair of points (i.e.,
    // x and x + n/2) into different halves of the array.
    // Then, assuming permutations on the two halves, at the end we need only
    // swap the two congruent and aligned elements into the correct slot.
    const auto follow = [&] (Nat start) {
      auto src = start;
      bool s = false;
      Nat srcmod = modn2(src);
      do {
        // mark the current point as visited
        visited[srcmod] = !visit_start;
        // set the permutation bit
        // as a special case, 0 is always statically known to be false and
        // need not be handled
        if (srcmod != 0) { buffer[srcmod - 1] = s; }
        // find the source whose target is congruent to tgt mod n/2
        const Nat tgt = src_to_tgt[src];
        const Nat ctgt = cong(tgt);
        const auto csrc = tgt_to_src[ctgt];
        // swap the congruent target's source if the source's output bucket
        // is currently equal to the congruent target's source's bucket
        s = ((s != (src >= n2)) == (csrc >= n2));
        // focus on the source congruent to the congruent target's source
        // which is now constrained by s
        src = cong(csrc);
        srcmod = modn2(src);
      } while (srcmod != modn2(start));
    };

    // special case: we always need to follow the permutation starting at 0
    follow(0);
    follow(n2);

    for (Nat cursor = 1; cursor < n2; ++cursor) {
      if (visited[cursor] == visit_start) {
        // follow the permutation only if we have not yet visited this point
        follow(cursor);
        follow(cursor + n2);
      }
    }

    // apply the first layer of swaps
    for (Nat i = 1; i < n2; ++i) {
      if (buffer[i-1]) {
        std::swap(src_to_tgt[i], src_to_tgt[i + n2]);
        std::swap(xs[i], xs[i + n2]);
      }
    }

    // program the final layer of swaps
    // place the programming into the end of the buffer such that it is not
    // overwritten by recursive calls
    for (Nat i = 0; i < n2; ++i) {
      auto& tgt0 = src_to_tgt[i];
      auto& tgt1 = src_to_tgt[i + n2];
      buffer[modn2(tgt0) + n2] = tgt0 >= n2;
      tgt0 = modn2(tgt0);
      tgt1 = modn2(tgt1);
    }

    // recursively permute the two halves
    permute(
        logn-1,
        src_to_tgt,
        tgt_to_src,
        !visit_start,
        visited,
        buffer,
        xs);
    permute(
        logn-1,
        src_to_tgt + n2,
        tgt_to_src + n2,
        !visit_start,
        visited + n4,
        buffer,
        xs + n2);

    // apply the final layer of swaps
    for (Nat i = 0; i < n2; ++i) {
      if (buffer[i + n2]) {
        std::swap(xs[i], xs[i + n2]);
      }
    }
  }
}


void permute(std::span<Nat> targets, std::span<Nat> xs) {
  const Nat n = targets.size();
  assert(xs.size() == n);
  if (n > 0) {
    const auto logn = log2(n);
    // permutation only set up for powers of two
    assert(n == (1 << logn));
    std::vector<bool> visited(n/2);
    std::vector<bool> programming_buffer(n);

    std::vector<Nat> tgt_to_src(n);
    for (Nat i = 0; i < n; ++i) { tgt_to_src[targets[i]] = i; }

    permute(
        logn,
        targets.data(),
        tgt_to_src.data(),
        false,
        visited,
        programming_buffer,
        xs.data());
  }
}

#endif
