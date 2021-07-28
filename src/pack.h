#ifndef PACK_H__
#define PACK_H__


#include "bit.h"


template <Role role, std::size_t n>
struct Pack {
public:
  static Pack compose(std::span<const Bit<role>>);
  void decompose(std::span<Bit<role>>) const;

private:
  std::bitset<128> label;
};


#endif
