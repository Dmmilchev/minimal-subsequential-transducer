#include "StateSignature.h"
#include <algorithm>

void StateSignature::print() const {
    if (isFinal) std::cout << "Final "; else std::cout << "isn't final ";
    if (isFinal) std::cout << "Psi: " << psiOutput << "   ";
    for (auto triple: transitions) {
        std::cout << std::get<0>(triple) << " " << std::get<1>(triple) << " "<< std::get<2>(triple) << "   ";
    }
    std::cout << std::endl;
}

StateSignature::StateSignature(Transducer& t, State q) {
    isFinal = t.finalStates.count(q) == 1;
    psiOutput = t.getPsi(q);

    auto it = t.dl.find(q);
    if (it != t.dl.end()) {
        for (const auto& letter : t.alphabet) {
            auto jt = it->second.find(letter);
            if (jt != it->second.end()) {
                State nextState = jt->second.first;
                Nat output = jt->second.second;
                transitions.emplace_back(letter, nextState, output);
            }
        }
    }
}

bool StateSignature::operator==(const StateSignature& other) const {
    if (isFinal != other.isFinal)
        return false;
    if (isFinal && psiOutput != other.psiOutput)
        return false;

    return transitions == other.transitions;
}

bool StateSignature::operator<(const StateSignature& other) const {
    if (isFinal != other.isFinal)
        return isFinal < other.isFinal;

    if (isFinal && psiOutput != other.psiOutput)
        return psiOutput < other.psiOutput;

    return transitions < other.transitions;
}