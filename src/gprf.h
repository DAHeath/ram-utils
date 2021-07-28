#ifndef GPRF_H__
#define GPRF_H__


#include "bit.h"


template <Role role>
struct GPRF {
public:
  static constexpr std::size_t key_size = 128;
  static constexpr std::size_t input_size = 128;
  static constexpr std::size_t output_size = 128;

  void operator()(std::span<const Bit<role>> input, std::span<Bit<role>> output);

private:
  std::vector<const Bit<role>> key;
};


// The specification of the prf is used by Gen to compute the same function as
// the garbled prf
void prf(
    std::span<const std::bitset<128>> key,
    std::span<const std::bitset<128>> input,
    std::span<std::bitset<128>> output) {
}



#endif
