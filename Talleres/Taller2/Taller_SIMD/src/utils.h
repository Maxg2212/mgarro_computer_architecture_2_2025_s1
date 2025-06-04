#ifndef UTILS_H
#define UTILS_H

#include <cstddef>
#include <random>
#include <string>
#include <cstdlib>
#include <cstring>

extern std::mt19937 rng;  

char random_char();
char* generate_random_string(size_t length, size_t alignment, bool zero_pad);
unsigned int parse_seed();
size_t get_memory_usage_kb();

#endif
