#include "bit.h"
#include "prg.h"


PRF Bit<Role::Gen>::prf;
std::bitset<128> Bit<Role::Gen>::delta;
std::size_t Bit<Role::Gen>::nonce = 0;
std::vector<std::bitset<128>> Bit<Role::Gen>::material;


PRF Bit<Role::Eval>::prf = Bit<Role::Gen>::prf;
std::size_t Bit<Role::Eval>::nonce = 0;
std::span<std::bitset<128>> Bit<Role::Eval>::material;
