#pragma once
#include <iostream>
#include <vector>
#include <map>
#include "Transducer.h"
#include "StateHashes.h"
#include "StateSignature.h"

using Word = std::vector<Letter>;

void reduceExcept(  Transducer& t, 
                    Word& word, 
                    size_t until, std::vector<State>& Tw,
                    std::map<StateSignature, State>& minimized);

void reduceClonedStatesExcept(  Transducer& t, 
            Word& word, 
            size_t until, std::vector<State>& Tw,
            std::unordered_map<State, State, StateHash, StateEqual>& minimized);
            
void addWord(Transducer& t, Word& wordToAdd, Nat wordOutput, std::vector<State>& Tw, size_t c);

std::pair<Transducer, std::map<StateSignature, State>> fromSortedDictionary(std::vector<std::pair<Word, Nat>>& dict);

void addArbitraryWord(Transducer& t, Word& word, Nat output, std::map<StateSignature, State>& minimized);

std::vector<State> traverseNotConvergent(Transducer& t, Word& word, std::map<StateSignature, State>& minimized);

void deleteWord(Transducer& t, Word& word, std::map<StateSignature, State>& minimized);