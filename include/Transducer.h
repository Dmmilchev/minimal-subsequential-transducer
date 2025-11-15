#pragma once 
#include <iostream>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <vector>


using State = unsigned int;
#define NotState UINT32_MAX

using Letter = char32_t;

using Nat = unsigned int;
#define NotNat UINT32_MAX

using Table = std::vector<std::pair<std::vector<Letter>, Nat>>;

struct Transducer {
    std::set<Letter> alphabet;
    uint32_t statesCount = 0;
    uint32_t stateMax = UINT32_MAX;
    std::unordered_set<State> finalStates;
    Nat iota = 0;
    std::unordered_map<State, Nat> psi;
    std::unordered_map<State, std::unordered_map<Letter, std::pair<State, Nat>>> dl;
    std::unordered_map<State, size_t> refCount;
    std::unordered_set<State> deletedStates;


    State delta(State s, Letter l) const;
    Nat lambda(State s, Letter l) const;
    std::pair<State, Nat> getDL(State s, Letter l) const;
    Nat getPsi(State q) const;
    void setDeltaLambda(State s, Letter l, State next_state, Nat nat);
    void setDelta(State s, Letter l, State next_state);
    void setLambda(State s, Letter l, Nat nat);
    void setPsi(State s, Nat out);

    void removeDeltaLambda(State s, Letter l);

    void eraseTransitionFrom(State q);
    void removeState(State q);
    void clearState(State q);
    void removeFromFinal(State q);
    
    void makeFinal(State q);
    bool isFinal(State q) const;

    void addState();
    void addFinalState();
    void addKStates(size_t k);

    Nat traverse(std::vector<Letter> word);

    size_t getNumberOfOutgoingTransitions(State q);

    size_t getTransitionsCount();
    void print() const;
    void compactPrint() const;
};