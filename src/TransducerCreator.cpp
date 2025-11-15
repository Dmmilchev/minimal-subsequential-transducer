#include "TransducerCreator.h"
#include <cassert>
#include <algorithm>
#include "StateSignature.h"
#include "FileUtils.h"
#include <map>

void reduceExcept(Transducer& t, Word& word, size_t until, std::vector<State>& Tw,std::map<StateSignature, State>& minimized) {
    for (int i = Tw.size() - 1; i > until; --i) {
        // minimize Tw[i]
        StateSignature sign(t, Tw[i]);

        // StateSignature sign(t, Tw[i])
        if (minimized.count(sign) == 0) {
            minimized[sign] = Tw[i];
        }
        else {
            State r = minimized[sign];  // r is eq to Tw[i], r belongs to Q/Tw
            assert(t.getPsi(Tw[i]) == t.getPsi(r));
            for (Letter x : t.alphabet) {
                assert(t.lambda(Tw[i], x) == t.lambda(r, x));
                assert(t.delta(Tw[i], x) == t.delta(r, x));
            }

            t.setDelta(Tw[i - 1], word[i - 1], r);
            t.removeState(Tw[i]);
        }
        
        Tw.pop_back();
    }
}

void addWord(Transducer& t, Word& wordToAdd, Nat wordOutput, std::vector<State>& Tw, size_t c) {
    // this function adds a word into the language of the transducer iff t is minimal except for w,
    // where w is the longest common prefix between wordToAdd and dom(L(t)) and Tw is the state
    // sequence of w starting from q0 = 0;

    for (Letter x : wordToAdd) {
        t.alphabet.insert(x);
    }
    
    for (int i = c; i < wordToAdd.size() - 1; ++i) {
        t.addState();
        Tw.push_back(t.stateMax);
        t.setDeltaLambda(Tw[i], wordToAdd[i], Tw[i + 1], NotNat);
    }
    t.makeFinal(Tw.back());

    // modify outputs up to c
    Nat oldOutput = t.iota;
    t.iota = std::min(t.iota, wordOutput);
    Nat currOutput = t.iota;
    Nat leftOutput = wordOutput - currOutput;
    for (int i = 0; i < c; ++i) {
        if (t.finalStates.count(Tw[i]) == 1) {
            t.psi[Tw[i]] += oldOutput - currOutput; // !!!!
        }

        for (auto& p : t.dl[Tw[i]]) {
            if (p.first != wordToAdd[i]) {
                assert(oldOutput >= currOutput);
                p.second.second += oldOutput - currOutput;
            }
        }
        oldOutput += t.lambda(Tw[i], wordToAdd[i]);
        t.setLambda(Tw[i], wordToAdd[i], std::min(t.lambda(Tw[i], wordToAdd[i]), leftOutput));
        currOutput += t.lambda(Tw[i], wordToAdd[i]);
        leftOutput -= t.lambda(Tw[i], wordToAdd[i]);
    }

    if (c == wordToAdd.size() - 1) {
        t.setPsi(Tw[c], leftOutput);
    }
    else {
        t.setLambda(Tw[c], wordToAdd[c], leftOutput);
    }

    // this if is only possible if adding arbitrary word
    if (c != wordToAdd.size() - 1 && t.finalStates.count(Tw[c]) == 1) {
        // print_char32_vector(wordToAdd); std::cout << std::endl;
        t.setPsi(Tw[c], t.getPsi(Tw[c]) + oldOutput - currOutput);
    }

    for (auto& p : t.dl[Tw[c]]) {
        if (p.first != wordToAdd[c]) {
            assert(oldOutput >= currOutput);
            p.second.second += oldOutput - currOutput;
        }
    }

    // modify output after c
    for (int i = c + 1; i < Tw.size() - 1; ++i) {
        t.setLambda(Tw[i], wordToAdd[i], 0);
    }
}


std::pair<Transducer, std::map<StateSignature, State>> fromSortedDictionary(std::vector<std::pair<Word, Nat>>& dict) {
    Transducer t;

    // create alphabet. We need the alphabet in sorted order in the state hash function,
    // which is used to distinguish equivalent states.
    std::unordered_set<Letter> tempAlphabet;
    for (auto p : dict) {
        for (int i = 0; i < p.first.size() - 1; ++i) {
            tempAlphabet.insert(p.first[i]);
        }
    }
    for (const Letter& l : tempAlphabet){
        t.alphabet.insert(l);
    }

    //create initial transducer
    t.addState();
    Word& word = dict[0].first;
    Nat out = dict[0].second;

    t.iota = out;
    std::vector<State> Tw; Tw.push_back(t.stateMax);
    for (int i = 0; i < word.size() - 1; ++i) {
        t.addState(); 
        Tw.push_back(t.stateMax);
        t.setDeltaLambda(Tw[i], word[i], Tw[i+1], 0);
    }
    t.makeFinal(Tw.back());

    std::map<StateSignature, State> minimized;
    minimized[StateSignature(t, 0)] = 0;
    for (int k = 1; k < dict.size(); ++k) {
        Word& currWord = dict[k].first;
        Word& prevWord = dict[k-1].first;
        Nat currWordOutput = dict[k].second;

        // get common prefix
        size_t c = 0;
        while(currWord[c] == prevWord[c])  
            c++;
         
        // reduce until the common prefix
        reduceExcept(t, prevWord, c, Tw, minimized);

        // add the new word to language
        addWord(t, currWord, currWordOutput, Tw, c);
    }  

    // final step - reduce to eps
    reduceExcept(t, dict.back().first, 0, Tw, minimized);

    return std::pair<Transducer, std::map<StateSignature, State>>(t, minimized);
}

std::vector<State> traverseNotConvergent(Transducer& t, Word& word, std::map<StateSignature, State>& minimized) {
    // Returns the states sequence Tw, such that Tw is the maximum path traversing the transducer with word
    // and visiting only non convergent states.
    // Removes each states from Tw, from minimized, because
    // after the procedure we consider t to be minimal except for word[0, ..., Tw.size() - 1];

    std::vector<State> Tw; Tw.push_back(0);
    minimized.erase(StateSignature(t, Tw[0]));
    for (int i = 0; i < word.size() - 1; ++i) {
        State next = t.delta(Tw[i], word[i]);
        if (next != NotState && t.refCount[next] == 1){
            assert(t.refCount[next] != 0);
            Tw.push_back(next);
            minimized.erase(StateSignature(t, next));
        }
        else {
            break;
        }
    }
    return Tw;
}

void increaseFrom(Transducer& t, Word& word, std::vector<State>& Tw, std::map<StateSignature, State>& minimized, size_t from) {
    for(int i = from; i < word.size() - 1; ++i) {
        assert(i == Tw.size() - 1);

        State next = t.delta(Tw[i], word[i]);
        if (next != NotState) {
            // std::cout << "cloning  " << word[i] << "  ";
            // cloning state
            t.addState();
            t.setDelta(Tw[i], word[i], t.stateMax);
            for (const auto& p : t.dl[next]) {
                Letter l = p.first;
                State q = p.second.first;
                Nat out = p.second.second;
                t.setDeltaLambda(t.stateMax, l, q, out);
            }
            if (t.isFinal(next)) {
                t.makeFinal(t.stateMax);
                t.setPsi(t.stateMax, t.getPsi(next));
            }
            Tw.push_back(t.stateMax);
        }
        else break;
    }
}

void addArbitraryWord(Transducer& t, Word& word, Nat output, std::map<StateSignature, State>& minimized) {
    // add a random word to the language of the transducer with given output.
    // this function does not require word to be lexicographically bigger 
    // than  the words in dom(L(t)).
    // shouldn't add words which are already in the dom(L(t))

    std::vector<State> Tw = traverseNotConvergent(t, word, minimized);

    int nonConv = Tw.size() - 1;  // Tw[0, ... nonConv] are non convergent states
    increaseFrom(t, word, Tw, minimized, nonConv);

    int cloned = Tw.size() - 1;  // Tw[nonConv + 1, ... cloned] are cloned states
    addWord(t, word, output, Tw, cloned);

    assert(t.deletedStates.count(0) == 0);

    reduceExcept(t, word, 0, Tw, minimized);
}

void deleteWord(Transducer& t, Word& word, std::map<StateSignature, State>& minimized) {
    std::vector<State> Tw = traverseNotConvergent(t, word, minimized);
    int nonConv = Tw.size() - 1;  // Tw[0, ... nonConv] are non convergent states

    increaseFrom(t, word, Tw, minimized, nonConv);
    int cloned = Tw.size() - 1;  // Tw[nonConv + 1, ... cloned] are cloned states

    t.removeFromFinal(Tw.back());

    // delete useless states
    int i;
    for (i = cloned; i > 0; --i) {
        if (t.getNumberOfOutgoingTransitions(Tw[i]) == 0 && !t.isFinal(Tw[i])) {
            t.removeDeltaLambda(Tw[i - 1], word[i - 1]);
            t.removeState(Tw[i]);
            Tw.pop_back();
        }
        else break;
    }

    // pull output in the front (make canonical)
    std::vector<Nat> increasedBy;
    for (; i > 0; --i) {
        Nat min = NotNat; // max value
        for (Letter x: t.alphabet) {
            min = std::min(min, t.lambda(Tw[i], x));
        }

        if (t.isFinal(Tw[i])) {
            min = std::min(min, t.getPsi(Tw[i]));
        }

        if (min != NotNat){
            t.setLambda(Tw[i - 1], word[i - 1], t.lambda(Tw[i - 1], word[i - 1]) + min);
            increasedBy.push_back(min);
        }

        for (Letter x: t.alphabet) {
            if (t.lambda(Tw[i], x) != NotState) {
                t.setLambda(Tw[i], x, t.lambda(Tw[i], x) - min);
            }
        }
    }

    // pull output in iota
    Nat min = NotNat; // max value
    for (Letter x: t.alphabet) {
        min = std::min(min, t.lambda(0, x));
    }

    if (min != NotNat){
        t.iota += min;
    }

    for (Letter x: t.alphabet) {
        if (t.lambda(0, x) != NotState) {
            t.setLambda(0, x, t.lambda(0, x) - min);
        }
    }

    std::reverse(increasedBy.begin(), increasedBy.end());
    for (int i = 0; i < increasedBy.size(); ++i) {
        if (t.isFinal(Tw[i + 1])) {
            t.setPsi(Tw[i + 1], t.getPsi(Tw[i + 1]) - increasedBy[i]);
        }
    }

    reduceExcept(t, word, 0, Tw, minimized);
}