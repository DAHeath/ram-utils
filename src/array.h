#ifndef ARRAY_H__
#define ARRAY_H__


#include "bit.h"
#include "room.h"
#include "gprf.h"
#include "nat.h"
#include "scan.h"

#include <array>


enum class ArrayKind {
  Recursive,
  SquareRoot,
  LinearScan
};


/* using Vec = std::vector<std::bitset<128>>; */
/* using Region = std::span<std::bitset<128>>; */


template <Nat block_size, // how many bits per block
          Nat bits_per_pack> // how many bits per packed label
constexpr ArrayKind select_kind(Nat nbits) {
  // TODO choose appropriately
  if (nbits < 128) {
    return ArrayKind::LinearScan;
  } else {
    return ArrayKind::Recursive;
  }
}


template <Role role,
          Nat words_per_block, // how many words per block
          Nat bits_per_pack, // how many bits per packed label
          Nat bits_per_word, // the word bit-width
          Nat words> // the number of words
struct Array;


template <Nat blocks>
constexpr bool occupied(Nat clock, Nat level) {
  // TODO
  return true;
}


template <Role role>
void mask_multiplex(
    Nat level,
    Nat clock,
    std::span<const Bit<role>> garbled_level,
    std::span<const Bit<role>> prf_output,
    Nat& out_ix,
    std::span<std::bitset<128>> out) {
  // TODO
}


template <Role role,
          Nat words_per_block,
          Nat bits_per_pack,
          Nat bits_per_word,
          Nat words>
struct Array {
private:
  using Bit = Bit<role>;

public:
  static constexpr Nat bits_per_block = words_per_block * bits_per_word;
  static constexpr Nat labels_per_block = ceil_div(bits_per_block, bits_per_pack);
  static constexpr Nat blocks = ceil_div(words, words_per_block);

  static_assert(is_pow2(words_per_block),
      "Each block must have a power of two words so that the index can be efficiently split.");


private:
  // Fetch a particular block from RAM and store in the stash
  void stash_block(std::span<const Bit> which) {
    assert (which.size() == log2(blocks));

    // read the time table to find the last time this index was written
    std::array<Bit, log2(max_timestamp)> timestamp;
    timetable.read(which, timestamp);

    // mark the old time as stale
    const auto truth = Bit::constant(true);
    stale.write(timestamp, std::span { &truth, 1 });

    // compute the level (in unary) that contains the looked up word
    const auto level = deduce_level<role, blocks>(clock, timestamp);

    // compute the mask for the looked up block via a prf
    // F_k(level | timestamp)
    std::vector<Bit> prf_input(GPRF<role>::input_size);
    std::vector<Bit> prf_output(GPRF<role>::output_size);
    std::copy(level.begin(), level.end(), prf_input.begin());
    std::copy(timestamp.begin(), timestamp.end(), prf_input.begin() + level.size());
    f_k(prf_input, prf_output);

    // read each level and multiplex the blocks
    Nat level_ix;
    std::vector<std::bitset<128>> mask(ceil_div(bits_per_block, bits_per_pack));
    std::vector<std::bitset<128>> buf(bits_per_block);
    bool first = true;
    for (Nat l = 0; l < log2(blocks) + 1; ++l) {
      // only ~1/2 of the levels will hold elements on a given clock tick
      if (occupied<blocks>(clock, l)) {
        // multiplex the prf call with Gen's statically known dummy prf output
        mask_multiplex<role>(l, clock, level[l], prf_output, level_ix, mask);

        if (first) {
          // on the first accessed level, there is no need to multiplex
          // just store straight into the stash
          rooarrays[l].read(level_ix, mask, stash);
          first = false;
        } else {
          rooarrays[l].read(level_ix, mask, buf);
          // multiplex the output
          for (Nat i = 0; i < bits_per_block; ++i) {
            stash[i] ^= level[l] & (stash[i] ^ buf[i]);
          }
        }
      }
    }
  }

public:
  void write(
      std::span<const Bit> which,
      std::span<const Bit> what) {
    assert(what.size() == bits_per_word);
    assert(which.size() == log2(words));

    const auto which_block = which.subspan(log2(words_per_block));
    const auto which_word = which.subspan(0, log2(words_per_block));

    stash_block(which_block);

    std::vector<Bit> the_word(bits_per_word);
    Scan::write(stash, which_word, the_word);
  }


  void read(
      std::span<const Bit> which,
      std::span<Bit> out) {
    assert(out.size() == bits_per_word);
    assert(which.size() == log2(words));

    const auto which_block = which.subspan(log2(words_per_block));
    const auto which_word = which.subspan(0, log2(words_per_block));

    stash_block(which_block);

    std::vector<Bit> the_word(bits_per_word);
    Scan::read(stash, which_word, out);
  }


private:
  // There are 2n timestamps before the array is recycled
  static constexpr Nat max_timestamp = 2*words;
  static constexpr Nat n_levels = log2(blocks)+1;

  Nat clock;
  std::vector<std::bitset<128>> stash;
  std::vector<std::bitset<128>> memory;
  std::array<ROOM<role, labels_per_block, bits_per_pack>, n_levels> rooarrays;
  GPRF<role> f_k;

  // For each block, store the timestamp when it was last written.
  Array<
    role,
    ceil_div(bits_per_block, log2(max_timestamp)),
    bits_per_pack,
    log2(max_timestamp),
    words> timetable;

  // For each timestamp, indicate if the timestamp is superceded by another access.
  Array<role, bits_per_block, bits_per_pack, 1, max_timestamp> stale;

  std::array<Bit, n_levels> deduce_level(Nat clock, std::span<const Bit> timestamp) {
    // TODO
  }
};

#endif
