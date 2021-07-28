#ifndef GATE_H__
#define GATE_H__


#include <span>
#include "prf.h"


using Label = std::bitset<128>;


Label gbAnd(
    const PRF& f,
    const Label& delta,
    const Label& A0,
    const Label& B0,
    Label& mat0,
    Label& mat1,
    std::size_t nonce);

Label evAnd(
    const PRF& f,
    const Label& A,
    const Label& B,
    const Label& mat0,
    const Label& mat1,
    std::size_t nonce);


#endif
