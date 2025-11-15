#pragma once
#include <iostream>
#include <vector>
#include <unordered_map>
#include "Transducer.h"
#include "StateHashes.h"

using Word = std::vector<Letter>;

std::pair<Transducer, std::unordered_map<State, State, StateHash, StateEqual>> fromSortedDictionary2(std::vector<std::pair<Word, Nat>>& dict);
void reduceExcept2(  Transducer& t, 
                    Word& word, 
                    size_t until, std::vector<State>& Tw,
                   std::unordered_map<State, State, StateHash, StateEqual>& minimized);

            
void addWord2(Transducer& t, Word& wordToAdd, Nat wordOutput, std::vector<State>& Tw, size_t c);

std::pair<Transducer, std::unordered_map<State, State, StateHash, StateEqual>> fromSortedDictionary2(std::vector<std::pair<Word, Nat>>& dict);

void addArbitraryWord2(Transducer& t, Word& word, Nat output, std::unordered_map<State, State, StateHash, StateEqual>& minimized);

std::vector<State> traverseNotConvergent2(Transducer& t, Word& word, std::unordered_map<State, State, StateHash, StateEqual>& minimized);

void deleteWord2(Transducer& t, Word& word, std::unordered_map<State, State, StateHash, StateEqual>& minimized);