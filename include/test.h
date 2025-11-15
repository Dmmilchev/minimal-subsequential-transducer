#pragma once
#include <vector>
#include "Transducer.h"

using E = std::pair<std::vector<Letter>, Nat>;

// Helper function to compare two Elements
bool elementsEqual(const E& a, const E& b);

// Returns elements in v1 that are NOT in v2
std::vector<E> differenceVector(
    const std::vector<E>& v1,
    const std::vector<E>& v2);

// Returns vector with Vs from both v1 and v2 (concatenation)
std::vector<E> unionVector(
    const std::vector<E>& v1,
    const std::vector<E>& v2);

void testAngel();
void myTest1();
void myTest2();
void myTest3();
void myTest4();
void myTest5();
void myTest6();

void exam1();

void testPM();

void test0();

void test1();

void test3();