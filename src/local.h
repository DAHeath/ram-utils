#ifndef LOCAL_H__
#define LOCAL_H__


#include "bit.h"
#include "prg.h"


// Locally simulate a GC interaction between Gen and Eval.
template <typename F, typename G>
std::vector<bool> local(const F& fg, const G& fe, const std::vector<bool>& inp) {
  // Gen's random tape
  PRG g;

  // The fixed key
  PRF f;

  Bit<Role::Gen>::prf = f;
  Bit<Role::Gen>::delta = g() | std::bitset<128> { 1 };
  Bit<Role::Gen>::nonce = 0;
  Bit<Role::Gen>::material.resize(0);

  std::vector<Bit<Role::Gen>> g_inp(inp.size());
  for (auto& i : g_inp) { i = g(); }

  const std::vector<Bit<Role::Gen>> g_out = fg(std::span<const Bit<Role::Gen>> { g_inp });

  Bit<Role::Eval>::prf = f;
  Bit<Role::Eval>::nonce = 0;
  Bit<Role::Eval>::material = Bit<Role::Gen>::material;

  std::vector<Bit<Role::Eval>> e_inp(inp.size());
  for (std::size_t i = 0; i < inp.size(); ++i) {
    if (inp[i]) {
      e_inp[i] = g_inp[i].label ^ Bit<Role::Gen>::delta;
    } else {
      e_inp[i] = g_inp[i].label;
    }
  }

  const std::vector<Bit<Role::Eval>> e_out = fe(std::span<const Bit<Role::Eval>> { e_inp });

  if (g_out.size() != e_out.size()) {
    std::cerr << "Generator output size does not match Evaluator output size\n";
    std::exit(1);
  }

  std::vector<bool> out(e_out.size());
  for (std::size_t i = 0; i < out.size(); ++i) {
    if (e_out[i].label == g_out[i].label) {
      out[i] = false;
    } else if (e_out[i].label == (g_out[i].label ^ Bit<Role::Gen>::delta)) {
      out[i] = true;
    } else {
      std::cerr << "Illegal evaluator output\n";
      std::cerr << e_out[i].label << '\n';
      std::cerr << g_out[i].label << '\n';
      std::cerr << (g_out[i].label ^ Bit<Role::Gen>::delta) << '\n';
      std::exit(1);
    }
  }
  return out;
}


#endif
