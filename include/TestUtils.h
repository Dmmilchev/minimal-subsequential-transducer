#pragma once
#include "Transducer.h"

bool isStateCanonical(Transducer& t, State q);
bool isTransducerCanonical(Transducer& t);
void helper(Transducer& t, std::vector<Letter>& word, std::unordered_set<State>& visited, Letter l, State curr);
Table getLanguage(Transducer& t);
