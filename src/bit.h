#ifndef BIT_H__
#define BIT_H__


#include <bitset>
#include <vector>
#include <span>
#include <iostream>
#include "prf.h"
#include "gate.h"


enum class Role { Gen, Eval };


template <Role R>
struct Bit;

template <>
struct Bit<Role::Gen> {
public:
  constexpr Bit(const std::bitset<128>& label) noexcept : label(label) { }
  constexpr Bit() noexcept : label(0) { }


  static constexpr Bit constant(bool b) noexcept { return Bit { b ? delta : 0 }; }


  Bit operator~() const { return { label ^ delta }; }
  Bit& operator^=(const Bit& o) { label ^= o.label; return *this; }
  Bit& operator&=(const Bit& o) {
    material.resize(material.size() + 2);
    auto& mat0 = material[material.size()-2];
    auto& mat1 = material[material.size()-1];
    label = gbAnd(prf, delta, label, o.label, mat0, mat1, nonce);
    nonce += 2;
    return *this;
  }


  friend std::ostream& operator<<(std::ostream& os, const Bit& x) {
    os << x.label;
    return os;
  }


  static PRF prf;
  static std::bitset<128> delta;
  static std::size_t nonce;
  static std::vector<std::bitset<128>> material;

  std::bitset<128> label;
};


template <>
struct Bit<Role::Eval> {
public:
  constexpr Bit(const std::bitset<128>& label) noexcept : label(label) { }
  constexpr Bit() noexcept : label(0) { }


  static constexpr Bit constant(bool b) noexcept { return Bit { 0 }; }


  Bit operator~() const { return *this; }
  Bit& operator^=(const Bit& o) { label ^= o.label; return *this; }
  Bit& operator&=(const Bit& o) {
    auto& mat0 = material[0];
    auto& mat1 = material[1];
    material = material.subspan(2);
    label = evAnd(prf, label, o.label, mat0, mat1, nonce);
    nonce += 2;
    return *this;
  }


  friend std::ostream& operator<<(std::ostream& os, const Bit& x) {
    os << x.label;
    return os;
  }


  static PRF prf;
  static std::size_t nonce;
  static std::span<std::bitset<128>> material;

  std::bitset<128> label;
};


template <Role R>
void sendMaterial() {
  Bit<Role::Eval>::material = Bit<Role::Gen>::material;
}


template <Role R>
Bit<R>& operator&=(Bit<R>& l, bool r) {
  if (!r) {
    l = Bit<R>::constant(0);
  }
  return l;
}


template <Role R>
Bit<R> operator^(const Bit<R>& l, const Bit<R>& r) {
  auto out = l;
  out ^= r;
  return out;
}


template <Role R>
Bit<R> operator&(const Bit<R>& l, const Bit<R>& r) {
  auto out = l;
  out &= r;
  return out;
}


template <Role R>
Bit<R>& operator|=(Bit<R>& l, const Bit<R>& r) {
  l = ~l;
  l &= ~r;
  l = ~l;
  return l;
}


template <Role R>
Bit<R> operator|(const Bit<R>& l, const Bit<R>& r) {
  auto out = l;
  out |= r;
  return out;
}


template <Role R>
Bit<R> operator!=(const Bit<R>& l, const Bit<R>& r) { return l ^ r; }

template <Role R>
Bit<R> operator==(const Bit<R>& l, const Bit<R>& r) { return ~(l != r); }

template <Role R>
Bit<R> operator<=(const Bit<R>& l, const Bit<R>& r) { return ~l | r; }

template <Role R>
Bit<R> operator<(const Bit<R>& l, const Bit<R>& r) { return ~l & r; }

template <Role R>
Bit<R> operator>=(const Bit<R>& l, const Bit<R>& r) { return r <= l; }

template <Role R>
Bit<R> operator>(const Bit<R>& l, const Bit<R>& r) { return r < l; }


#endif
