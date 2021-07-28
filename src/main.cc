#include "bit.h"
#include "local.h"
#include "array.h"
#include "unary.h"
#include "permute.h"
#include <set>


using Atom = std::pair<bool, Nat>;

std::ostream& operator<<(std::ostream& os, Atom a) {
  auto [v, n] = a;
  if (v) { os << 'y'; }
  os << n;
  return os;
}



struct Term {
  std::vector<Atom> atoms;

  Term operator&(const Term& o) const {
    std::vector<Atom> out;
    std::copy(atoms.begin(), atoms.end(), std::back_inserter(out));
    std::copy(o.atoms.begin(), o.atoms.end(), std::back_inserter(out));
    return { out };
  }

  friend std::ostream& operator<<(std::ostream& os, const Term& t) {
    for (auto a: t.atoms) { os << a; }
    return os;
  }
};


struct Expr {
  std::vector<Term> terms;

  Expr operator^(const Expr& o) const {
    std::vector<Term> out;
    std::copy(terms.begin(), terms.end(), std::back_inserter(out));
    std::copy(o.terms.begin(), o.terms.end(), std::back_inserter(out));
    return { out };
  }

  Expr operator&(const Expr& o) const {
    std::vector<Term> out;
    for (auto& t: terms) {
      for (auto& s: o.terms) {
        out.push_back(t & s);
      }
    }
    return { out };
  }

  static Expr constant(bool b) {
    return Expr { { { { { false, b } } } } };
  }

  static Expr variable(Nat n) {
    return Expr { { { { { true, n } } } } };
  }

  friend std::ostream& operator<<(std::ostream& os, const Expr& e) {
    if (e.terms.size() > 0) {
      for (std::size_t i = 0; i < e.terms.size() - 1; ++i) {
        os << e.terms[i] << " + ";
      }
      os << e.terms[e.terms.size()-1];
    }
    return os;
  }
};





template <Role role>
std::vector<Bit<role>> test(std::span<const Bit<role>> inp) {
  const auto& x = inp[0];
  const auto& y = inp[1];
  const auto& z = inp[2];

  std::vector<Bit<role>> out(1);
  out[0] = x & y;
  return out;
}


int main() {
  /* std::vector<Nat> tgts = { 4, 1, 2, 3, 5, 7, 0, 6 }; */
  /* std::vector<Nat> tgts = { 7, 6, 5, 4, 3, 2, 1, 0 }; */
  std::vector<Nat> tgts = { 15, 7, 9, 0, 4, 13, 3, 2, 14, 11, 8, 1, 6, 5, 10, 12, };
  std::vector<Nat> perm = tgts;

  permute(tgts, perm);

  /* permute(programming, perm); */

  for (auto t : perm) {
    std::cout << t << ' ';
  } std::cout << '\n';

}
