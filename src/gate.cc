#include "gate.h"


Label gbAnd(
    const PRF& f,
    const Label& delta,
    const Label& A,
    const Label& B,
    Label& mat0,
    Label& mat1,
    std::size_t nonce) {
  const auto nonce0 = Label { nonce };
  const auto nonce1 = Label { nonce + 1 };
  const auto hA0 = f(A ^ nonce0);
  const auto hA1 = f(A ^ delta ^ nonce0);
  const auto hB0 = f(B ^ nonce1);
  const auto hB1 = f(B ^ delta ^ nonce1);
  const auto A0D = A & delta;
  const auto B0D = B & delta;

  const auto X = A[0] ? (hA1 ^ B0D) : hA0;
  const auto Y = B[0] ? (hB1 ^ A0D) : hB0;

  mat0 = hA0 ^ hA1 ^ B0D;
  mat1 = hB0 ^ hB1 ^ A0D;

  return A&B ^ X ^ Y;
}


Label evAnd(
    const PRF& f,
    const Label& A,
    const Label& B,
    const Label& mat0,
    const Label& mat1,
    std::size_t nonce) {
  const auto nonce0 = Label { nonce };
  const auto nonce1 = Label { nonce + 1 };
  const auto hA = f(A ^ nonce0);
  const auto hB = f(B ^ nonce1);
  const auto X = A[0] ? hA ^ mat0 : hA;
  const auto Y = B[0] ? hB ^ mat1 : hB;
  return (A&B) ^ X ^ Y;
}


