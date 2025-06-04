#include <iostream>
#include <chrono>
#include "utils.h"

size_t count_serial(const char* str, size_t len, char ch) {
    size_t count = 0;
    for (size_t i = 0; i < len; ++i)
        if (str[i] == ch)
            ++count;
    return count;
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        std::cerr << "Uso: " << argv[0] << " <length> <alignment> <char>\n";
        return 1;
    }

    size_t len = std::stoul(argv[1]);
    size_t align = std::stoul(argv[2]);
    char target = argv[3][0];

    rng.seed(parse_seed());
    char* data = generate_random_string(len, align, true);
    if (!data) return 1;


    size_t mem_before = get_memory_usage_kb();
    auto t1 = std::chrono::high_resolution_clock::now();
    size_t result = count_serial(data, len, target);
    auto t2 = std::chrono::high_resolution_clock::now();
    size_t mem_after = get_memory_usage_kb();

    size_t mem_used = mem_after > mem_before ? mem_after - mem_before : 0;
    std::chrono::duration<double> elapsed = t2 - t1;

    std::cout << "Character '" << target << "' found " << result << " times.\n";
    std::cout << "Execution time: " << elapsed.count() * 1e6 << " microseconds\n";
    std::cout << "Memory used (VmRSS): " << mem_used << " KB\n";


    free(data);
    return 0;
}
