#ifndef SCAN_H__
#define SCAN_H__

#include "nat.h"
#include "bit.h"
#include "unary.h"

namespace Scan {

template <typename Bool, Nat block_size>
void read(
    std::span<const Bool> memory,
    std::span<const Bool> index,
    std::span<Bool> out) {
  assert(out.size() == block_size);
  assert(divides(block_size, memory.size()));

  const auto blocks = memory.size()/block_size;
  const auto flags = unary(index);

  std::copy(memory.begin(), memory.begin() + block_size, out.begin());
  for (Nat i = 1; i < blocks; ++i) {
    auto slice = memory.subspan(i*block_size, block_size);
    for (std::size_t j = 0; j < block_size; ++j) {
      out[j] ^= flags[i] & (out[j] ^ slice[j]);
    }
  }
}


template <typename Bool, Nat block_size>
void write(
    std::span<Bool> memory,
    std::span<const Bool> index,
    std::span<const Bool> to_write) {
  assert(to_write.size() == block_size);
  assert(divides(block_size, memory.size()));

  const auto blocks = memory.size()/block_size;
  const auto flags = unary(index);

  for (Nat i = 0; i < blocks; ++i) {
    auto slice = memory.subspan(i*block_size, block_size);
    for (std::size_t j = 0; j < block_size; ++j) {
      slice[j] ^= flags[i] & (to_write[j], slice[j]);
    }
  }
}

}

#endif
