#pragma once

#include <iostream>
#include "Transducer.h"
#include <vector>
#include <tuple>

struct StateSignature {
    bool isFinal;
    Nat psiOutput;
    std::vector<std::tuple<Letter, State, Nat>> transitions;

    StateSignature(Transducer& t, State q);
    void print() const;

    bool operator==(const StateSignature& other) const;
    bool operator<(const StateSignature& other) const;
};