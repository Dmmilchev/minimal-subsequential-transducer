#include "Transducer.h"
#include <cassert>
#include <exception>
#include "FileUtils.h"

State Transducer::delta(State s, Letter l) const {
    auto it = dl.find(s);
    if (it != dl.end()) {
        auto jt = it->second.find(l);
        if (jt != it->second.end()) return jt->second.first;
    }
    return NotState;
}

Nat Transducer::lambda(State s, Letter l) const {
    auto it = dl.find(s);
    if (it != dl.end()) {
        auto jt = it->second.find(l);
        if (jt != it->second.end()) return jt->second.second;
    }
    return NotNat;
}

std::pair<State, Nat> Transducer::getDL(State s, Letter l) const {
    auto it = dl.find(s);
    if (it != dl.end()) {
        auto jt = it->second.find(l);
        if (jt != it->second.end()) return jt->second;
    }
    return std::make_pair(NotState, NotNat);
}

Nat Transducer::getPsi(State q) const {
    if (psi.count(q) == 0)
        return NotNat;
    else
        return psi.at(q);
}

void Transducer::setDeltaLambda(State s, Letter l, State next_state, Nat nat) {
    assert(dl[s].count(l) == 0);
    dl[s][l] = std::make_pair(next_state, nat);
    refCount[next_state] ++;
}

void Transducer::setDelta(State s, Letter l, State next_state) {
    assert(dl[s].count(l) > 0);
    refCount[delta(s, l)]--;
    if (refCount[delta(s, l)] == 0) {
        refCount.erase(delta(s, l));
    }
    dl[s][l].first = next_state;
    refCount[next_state]++;
}

void Transducer::setLambda(State s, Letter l, Nat nat) {
    assert(dl[s].count(l) > 0);
    dl[s][l].second = nat;
}

void Transducer::setPsi(State s, Nat out) {
    assert(finalStates.count(s) > 0);
    psi[s] = out;
}

void Transducer::removeDeltaLambda(State s, Letter l) {
    assert(dl[s].count(l) > 0);
    State next = dl[s][l].first;
    refCount[next]--;
    if (refCount[next] == 0)
        refCount.erase(next);
    dl[s].erase(l);
}

void Transducer::removeFromFinal(State q) {
    finalStates.erase(q);
    psi.erase(q);
}

void Transducer::removeState(State q) {
    assert(statesCount > 0);
    assert(refCount[q] == 0);

    clearState(q);
    removeFromFinal(q);
    refCount.erase(q);        
    statesCount--;

    if (q == stateMax) {
        stateMax--;
    }
    else {
        deletedStates.insert(q);
    }
}


void Transducer::clearState(State q) {
    for (const auto& p : dl[q]) {
        State nextState = p.second.first;
        refCount[nextState] -= 1;
        if (refCount[nextState] == 0)
            refCount.erase(nextState);
    }
    dl.erase(q);
}

void Transducer::addState() {
    ++stateMax;
    ++statesCount;
}

void Transducer::addFinalState() {
    addState();
    finalStates.insert(stateMax);
}

void Transducer::addKStates(size_t k) {
    stateMax += k;
    statesCount += k;
}

void Transducer::makeFinal(State q) {
    finalStates.insert(q);
    setPsi(q, 0);
}

bool Transducer::isFinal(State q) const{
    return finalStates.count(q) > 0;
}

Nat Transducer::traverse(std::vector<Letter> word) {
    Nat res = iota;
    State curr = 0;

    for (int i = 0; word[i] != '\0'; ++i) {
        Nat out = lambda(curr, word[i]);
        res += out;
        curr = delta(curr, word[i]);
        if (curr == NotState) {
            std::cout<<"Reached Not state" << std::endl;
            return NotNat;
        }

        // if transition is defined, output should also be defined
        assert(out != NotNat);  
    }

    if (finalStates.find(curr) != finalStates.end()) {
        return res + getPsi(curr);
    }
    else  {
        std::cout<<"Reached not final" << std::endl;
        return NotNat;
    }
}

size_t Transducer::getNumberOfOutgoingTransitions(State q) {
    return dl[q].size();
}

size_t Transducer::getTransitionsCount() {
    size_t transitionCount = 0;
    for (const auto& [state, transitions] : dl) {
        transitionCount += transitions.size();
    }
    return transitionCount;
}

void Transducer::print() const {
    std::cout << "==== Pretty Print of Transducer Data ====\n";
    std::cout << "States Count: " << statesCount << "\n";
    std::cout << "Max State Value: " << stateMax << "\n";
    std::cout << "Initial State (iota): " << iota << "\n";

    std::cout << "\nFinal States:\n";
    for (const auto& s : finalStates) {
        std::cout << "  - " << s << "\n";
    }

    std::cout << "\nPsi Map (State → Nat):\n";
    for (const auto& [state, nat] : psi) {
        std::cout << "  " << state << " → " << nat << "\n";
    }

    std::cout << "\nTransition Function (dl):\n";
    for (const auto& [fromState, transitions] : dl) {
        std::cout << "  From State " << fromState << ":\n";
        for (const auto& [letter, pair] : transitions) {
            std::cout << "    On '" << char32_to_utf8(letter) << "' → " << pair.first << " (with Nat = " << pair.second << ")\n";
        }
    }

    for (const auto& p : refCount) {
        std::cout << " State " << p.first << std::endl;
        std::cout << " Ref count is: " << p.second << std::endl;
    }

    std::cout << "=======================================\n";
}

void Transducer::compactPrint() const {
    size_t transitionCount = 0;
    for (const auto& [state, transitions] : dl) {
        transitionCount += transitions.size();
    }

    std::cout << "==== Transducer Summary ====\n";
    std::cout << "Number of States      : " << statesCount << "\n";
    std::cout << "Number of Transitions : " << transitionCount << "\n";
    std::cout << "Number of Final States: " << finalStates.size() << "\n";
    std::cout << "===========================\n";
}