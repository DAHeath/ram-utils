#ifndef ROOM_H__
#define ROOM_H__


#include "bit.h"
#include "pack.h"

#include <unordered_map>


// Read only once (per index) memory

template <Role role,
          std::size_t block_width, // how many labels per block
          std::size_t pack_n> // how many bits per packed label
struct ROOM;

template <std::size_t block_width, std::size_t pack_n>
struct ROOM<Role::Eval, block_width, pack_n> {
public:
  void read(
      std::size_t fi, // prf(index)
      std::span<const std::bitset<128>> mask,
      std::span<Bit<Role::Eval>> out) {
    const auto region = memory.subspan(index_map[fi]*block_width, block_width);
    Pack<Role::Eval, pack_n> pack;
    for (std::size_t i = 0; i < block_width; ++i) {
      // unmask each piece of the region and interpret the result as an
      // encrypted field element
      pack = region[i] ^ mask[i];
      // decompose the element into bits and place the results in the output
      pack.decompose(out.subspan(i*pack_n, pack_n));
    }
  }

private:
  std::span<std::bitset<128>> memory;
  std::unordered_map<std::size_t, std::size_t> index_map;
};


template <std::size_t block_width, std::size_t pack_n>
struct ROOM<Role::Gen, block_width, pack_n> {
public:
  void read(
      std::size_t fi, // prf(index)
      std::span<const std::bitset<128>> mask,
      std::span<Bit<Role::Eval>> out) {
    // in contrast to Eval, Gen's ROOM read is trivial:
    Pack<Role::Eval, pack_n> pack;
    for (std::size_t i = 0; i < block_width; ++i) {
      // interpret the masked all zero prf string as a field element zero label
      pack = mask[i];
      // decompose the element into bits and place the results in the output
      pack.decompose(out.subspan(i*pack_n, pack_n));
    }
  }
};


#endif
