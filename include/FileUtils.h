#pragma once
#include <vector>
#include "Transducer.h"
#include <string>

std::vector<std::pair<const char*, Nat>> readCSV(const std::string& filename);
std::vector<std::pair<std::vector<Letter>, Nat>>read_utf8_csv(const std::string& filename);
size_t utf8_to_char32(const char* s, size_t max_len, char32_t& out);
std::vector<char32_t> read_utf8_file_to_char32_vector(const std::string& filename);
std::string char32_to_utf8(char32_t cp);
void print_char32_vector(const std::vector<char32_t>& vec);