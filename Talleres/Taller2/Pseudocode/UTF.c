#include <iostream>
#include <emmintrin.h>  
#include <smmintrin.h>  
#include <bitset>


int get_mask(__m128i simd_vector){
    mascara = _mm_movemask_epi8(simd_vector);
    return mascara;
}

int count_ones(int mask){
    int counter = 0;
    for (int i = 0; i < 16; ++i){
        if (mask & (1 << i)){
            counter++;
        }

    }
    return counter;
}


int count_char_simd(const char* str, size_t M, char char_x) {
    int total = 0;
    __m128i vect_x = _mm_set1_epi8(char_x);

    size_t i = 0;
    for (; i + 15 < M; i += 16) {
        __m128i vect_str = _mm_loadu_si128(reinterpret_cast<const __m128i*>(str + i));
        __m128i result = _mm_cmpeq_epi8(vect_str, vect_x);
        int mask = get_mask(result);
        total += count_ones(mask);
    }

    // Procesar los bytes restantes (M no múltiplo de 16)
    for (; i < M; ++i) {
        if (str[i] == char_x) {
            total++;
        }
    }

    return total;
}

int main(){
    alignas(16) char* str[16] = {'A','s','l','k','a','d','j',';','s','k','d',';','j',';','0','x'};
    
    size_t M = std::strlen(str);
    char char_x = ';';

    int ocurrencias = count_char_simd(str, M, char_x);

    std::cout << char_x << ocurrencias << "\n";
    return 0;
}
