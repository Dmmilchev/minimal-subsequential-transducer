#pragma once
#include <iostream>
#include "Transducer.h"


struct StateHash {
    static const uint64_t FNV_OFFSET_BASIS = 14695981039346656037ULL;
    static const uint64_t FNV_PRIME = 1099511628211ULL;

    const Transducer* transducer;

    StateHash(const Transducer* t);

    uint64_t fnv1a_hash(const char* data, size_t length) const;

    size_t operator()(const State& s) const;
};

struct StateEqual {
    const Transducer* transducer;

    StateEqual(const Transducer* t);

    bool operator()(const State& lhs, const State& rhs) const;
};
