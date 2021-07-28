#include <rapidcheck.h>
#include "bit.h"
#include "local.h"
#include "scan.h"

int main() {
  rc::check("AND correct", [](bool x, bool y) {
      const auto f = []<Role role>(std::span<const Bit<role>> inp) {
        std::vector<Bit<role>> out(1);
        out[0] = inp[0] & inp[1];
        return out;
      };

      std::vector<bool> inp = { x, y };
      const auto out = local(f, f, inp);
      RC_ASSERT(out[0] == (x && y));
  });

  rc::check("XOR correct", [](bool x, bool y) {
      const auto f = []<Role role>(std::span<const Bit<role>> inp) {
        std::vector<Bit<role>> out(1);
        out[0] = inp[0] ^ inp[1];
        return out;
      };

      std::vector<bool> inp = { x, y };
      const auto out = local(f, f, inp);
      RC_ASSERT(out[0] == (x != y));
  });
}
