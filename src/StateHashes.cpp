#include "StateHashes.h"

StateEqual::StateEqual(const Transducer* t) : transducer(t) {}

bool StateEqual::operator()(const State& lhs, const State& rhs) const {
  if (transducer->getPsi(lhs) != transducer->getPsi(rhs)) {
    return false;
  }

  for (Letter x : transducer->alphabet) {
    if (transducer->delta(lhs, x) != transducer->delta(rhs, x) || transducer->lambda(lhs, x) != transducer->lambda(rhs, x)) {
      return false;
    }
  }

  return true;
}

uint64_t StateHash::fnv1a_hash(const char* data, size_t length) const{
  uint64_t hash = FNV_OFFSET_BASIS;
  for (size_t i = 0; i < length; ++i) {
      hash ^= static_cast<uint64_t>(data[i]);
      hash *= FNV_PRIME;
  }
  return hash;
}

StateHash::StateHash(const Transducer* t) : transducer(t) { }

size_t StateHash::operator()(const State& s) const {
  uint64_t hash = FNV_OFFSET_BASIS;

  Nat psi = transducer->getPsi(s);
  const char* psiData = reinterpret_cast<const char*>(&psi);

  hash ^= fnv1a_hash(psiData, sizeof(psi));
  hash *= FNV_PRIME;

  for (Letter x : transducer->alphabet) {
    State next = transducer->delta(s, x);
    const char* nextData = reinterpret_cast<const char*>(&next);
    hash ^= fnv1a_hash(nextData, sizeof(next));
    hash *= FNV_PRIME;

    Nat out = transducer->lambda(s, x);
    const char* outData = reinterpret_cast<const char*>(&out);
    hash ^= fnv1a_hash(outData, sizeof(out));
    hash *= FNV_PRIME;
  }

  return static_cast<std::size_t>(hash);
}
