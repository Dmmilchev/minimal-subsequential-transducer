#include "FileUtils.h"
#include <fstream>
#include <sstream>
#include <utility>
#include <cstring>

// Functions in this file are copy pasted from chatGPT!

std::vector<std::pair<const char*, Nat>> readCSV(const std::string& filename) {
    std::vector<std::pair<const char*, Nat>> data;
    std::ifstream file(filename);
    std::string line;

    if (!file.is_open()) {
        throw std::runtime_error("Could not open file");
    }

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string strPart;
        std::string intPart;

        if (!std::getline(ss, strPart, ',')) continue;
        if (!std::getline(ss, intPart, ',')) continue;

        char* cstr = new char[strPart.size() + 1];
        std::strcpy(cstr, strPart.c_str());

        Nat num = std::stoul(intPart);

        data.emplace_back(cstr, num);
    }

    return data;
}

Nat parse_uint(const char32_t* start, size_t length) {
    Nat result = 0;
    for (size_t i = 0; i < length; ++i) {
        if (start[i] < U'0' || start[i] > U'9') {
            throw std::runtime_error("Invalid digit in number field");
        }
        result = result * 10 + (start[i] - U'0');
    }
    return result;
}

std::vector<std::pair<std::vector<Letter>, Nat>>
read_utf8_csv(const std::string& filename) {
    std::vector<Letter> buffer = read_utf8_file_to_char32_vector(filename);

    std::vector<std::pair<std::vector<Letter>, Nat>> result;

    size_t start = 0;
    size_t n = buffer.size();

    for (size_t i = 0; i <= n; ++i) {
        if (i == n || buffer[i] == U'\n') {
            if (i == start) {
                // Skip empty line
                start = i + 1;
                continue;
            }

            // Find comma
            size_t comma_pos = start;
            bool comma_found = false;
            for (; comma_pos < i; ++comma_pos) {
                if (buffer[comma_pos] == U',') {
                    comma_found = true;
                    break;
                }
            }
            if (!comma_found) {
                throw std::runtime_error("No comma found in line");
            }

            // String field [start, comma_pos)
            size_t str_len = comma_pos - start;
            if (str_len > 0 && buffer[comma_pos - 1] == U'\r') {
                --str_len;  // Remove trailing '\r'
            }

            // Number field [comma_pos+1, i)
            size_t num_start = comma_pos + 1;
            size_t num_len = i - num_start;
            if (num_len > 0 && buffer[num_start + num_len - 1] == U'\r') {
                --num_len;  // Remove trailing '\r'
            }

            if (num_len == 0) {
                throw std::runtime_error("Empty number field");
            }

            // Copy string field into vector with null terminator
            std::vector<Letter> str_vec(buffer.begin() + start, buffer.begin() + start + str_len);
            str_vec.push_back(U'\0');  // null-terminate

            Nat num = parse_uint(&buffer[num_start], num_len);

            result.emplace_back(std::move(str_vec), num);

            start = i + 1;
        }
    }

    // some files start with this character
    if (result[0].first[0] == 65279)
        result[0].first.erase(result[0].first.begin());

    return result;
}

size_t utf8_to_char32(const char* s, size_t max_len, char32_t& out) {
    unsigned char c = (unsigned char)s[0];
    if (c < 0x80) {
        out = c;
        return 1;
    }
    else if ((c & 0xE0) == 0xC0) {
        if (max_len < 2) throw std::runtime_error("Invalid UTF-8");
        unsigned char c1 = (unsigned char)s[1];
        if ((c1 & 0xC0) != 0x80) throw std::runtime_error("Invalid UTF-8");
        out = ((c & 0x1F) << 6) | (c1 & 0x3F);
        if (out < 0x80) throw std::runtime_error("Overlong encoding");
        return 2;
    }
    else if ((c & 0xF0) == 0xE0) {
        if (max_len < 3) throw std::runtime_error("Invalid UTF-8");
        unsigned char c1 = (unsigned char)s[1];
        unsigned char c2 = (unsigned char)s[2];
        if (((c1 & 0xC0) != 0x80) || ((c2 & 0xC0) != 0x80)) throw std::runtime_error("Invalid UTF-8");
        out = ((c & 0x0F) << 12) | ((c1 & 0x3F) << 6) | (c2 & 0x3F);
        if (out < 0x800) throw std::runtime_error("Overlong encoding");
        return 3;
    }
    else if ((c & 0xF8) == 0xF0) {
        if (max_len < 4) throw std::runtime_error("Invalid UTF-8");
        unsigned char c1 = (unsigned char)s[1];
        unsigned char c2 = (unsigned char)s[2];
        unsigned char c3 = (unsigned char)s[3];
        if (((c1 & 0xC0) != 0x80) || ((c2 & 0xC0) != 0x80) || ((c3 & 0xC0) != 0x80)) throw std::runtime_error("Invalid UTF-8");
        out = ((c & 0x07) << 18) | ((c1 & 0x3F) << 12) | ((c2 & 0x3F) << 6) | (c3 & 0x3F);
        if (out < 0x10000 || out > 0x10FFFF) throw std::runtime_error("Overlong or invalid code point");
        return 4;
    }
    else {
        throw std::runtime_error("Invalid UTF-8 start byte");
    }
}

std::vector<char32_t> read_utf8_file_to_char32_vector(const std::string& filename) {
    // Open file in binary mode
    std::ifstream file(filename, std::ios::binary);
    if (!file) throw std::runtime_error("Failed to open file");

    // Read all bytes
    std::vector<char> buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    size_t i = 0;
    std::vector<char32_t> result;
    while (i < buffer.size()) {
        char32_t cp;
        size_t consumed = utf8_to_char32(&buffer[i], buffer.size() - i, cp);
        result.push_back(cp);
        i += consumed;
    }
    return result;
}

std::string char32_to_utf8(char32_t cp) {
    std::string out;

    if (cp <= 0x7F) {
        out.push_back(static_cast<char>(cp));
    } else if (cp <= 0x7FF) {
        out.push_back(static_cast<char>(0xC0 | ((cp >> 6) & 0x1F)));
        out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
    } else if (cp <= 0xFFFF) {
        out.push_back(static_cast<char>(0xE0 | ((cp >> 12) & 0x0F)));
        out.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
        out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
    } else if (cp <= 0x10FFFF) {
        out.push_back(static_cast<char>(0xF0 | ((cp >> 18) & 0x07)));
        out.push_back(static_cast<char>(0x80 | ((cp >> 12) & 0x3F)));
        out.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
        out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
    }
    return out;
}

void print_char32_vector(const std::vector<char32_t>& vec) {
    std::string utf8_str;
    for (char32_t cp : vec) {
        utf8_str += char32_to_utf8(cp);
    }
    std::cout << utf8_str;
}