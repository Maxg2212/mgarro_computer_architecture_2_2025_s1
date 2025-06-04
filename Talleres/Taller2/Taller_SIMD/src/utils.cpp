#include "utils.h"
#include <iostream>
#include <chrono>
#include <fstream>
#include <string>

std::mt19937 rng;


char random_char() {
    static const char charset[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        ";:.,!?@#";
    static const size_t max_index = sizeof(charset) - 2;
    static thread_local std::uniform_int_distribution<> dist(0, max_index);
    return charset[dist(rng)];
}


char* generate_random_string(size_t length, size_t alignment, bool zero_pad) {
    size_t padded_length = zero_pad ? ((length + alignment - 1) / alignment) * alignment : length;
    char* buffer = nullptr;
    if (posix_memalign((void**)&buffer, alignment, padded_length)) {
        std::cerr << "Failed to allocate aligned memory.\n";
        return nullptr;
    }

    for (size_t i = 0; i < length; ++i)
        buffer[i] = random_char();

    if (zero_pad)
        std::memset(buffer + length, 0, padded_length - length);

    return buffer;
}

unsigned int parse_seed() {
    return static_cast<unsigned int>(
        std::chrono::system_clock::now().time_since_epoch().count()
    );
}

size_t get_memory_usage_kb() {
    std::ifstream status_file("/proc/self/status");
    std::string line;
    while (std::getline(status_file, line)) {
        if (line.rfind("VmRSS:", 0) == 0) {
            size_t value_kb;
            sscanf(line.c_str(), "VmRSS: %zu kB", &value_kb);
            return value_kb;
        }
    }
    return 0;
}
