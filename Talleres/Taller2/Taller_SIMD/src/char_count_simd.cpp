#include <iostream>
#include <chrono>
#include <immintrin.h>
#include "utils.h"

// SIMD-based implementation using SSE2 intrinsics (128-bit version)
size_t count_simd(const char* str, size_t len, char ch) {
    size_t count = 0;
    __m128i target = _mm_set1_epi8(ch);
    size_t i = 0;

    for (; i + 15 < len; i += 16) {
        __m128i chunk = _mm_loadu_si128(reinterpret_cast<const __m128i*>(&str[i]));
        __m128i cmp = _mm_cmpeq_epi8(chunk, target);
        int mask = _mm_movemask_epi8(cmp);
        count += __builtin_popcount(mask);
    }

    
    for (; i < len; ++i)
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
    char* data = generate_random_string(len, align, false);
    if (!data) return 1;

    size_t mem_before = get_memory_usage_kb();
    auto t1 = std::chrono::high_resolution_clock::now();
    size_t result = count_simd(data, len, target);
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
