#include "TestUtils.h"
#include <iostream>
#include <queue>

bool isStateCanonical(Transducer& t, State q) {
    Nat min = NotNat;
    for (Letter x : t.alphabet) {
        min = std::min(min, t.lambda(q, x));
    }
     
    if (t.isFinal(q)) {
        min = std::min(min, t.getPsi(q));
    }

    return min == 0 || min == NotNat;
}

bool isTransducerCanonical(Transducer& t) {
    std::queue<State> Q;
    Q.push(0); // q0
    std::unordered_set<State> visited;

    while (!Q.empty()) {
        State q = Q.front();
        Q.pop();

        if (!isStateCanonical(t, q)) {
            return false;
        }
        
        for (Letter x : t.alphabet) {
            State next = t.delta(q, x);
            if (next != NotState && visited.count(next) == 0) {
                Q.push(next);
                visited.insert(q);
            }
        }

    }

    return true;
}

void helper(Transducer& t, std::vector<Letter>& word, std::unordered_set<State>& visited, Letter l, State curr, Nat currOutput, Table& res) {
    visited.insert(curr);
    word.push_back(l);

    if (t.isFinal(curr)) {
        res.push_back(std::make_pair(word, currOutput + t.getPsi(curr)));
    }

    for (Letter x : t.alphabet) {
        if (t.delta(curr, x) != NotState) {
            helper(t, word, visited, x, t.delta(curr, x), currOutput + t.lambda(curr, x), res);
        }
    }

    word.pop_back();
}

Table getLanguage(Transducer& t) {
    Table res;
    std::unordered_set<State> visited;
    std::vector<Letter> word;
    State curr = 0;
    Nat currOutput = t.iota;
    helper(t, word, visited, 0, curr, currOutput, res);
    return res;
}