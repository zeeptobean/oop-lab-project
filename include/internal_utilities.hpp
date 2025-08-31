#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <string>
#include <algorithm>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

inline void internal_safe_memset(void *dst, int val, size_t sz) {
    void* (*const volatile volatile_memset)(void*, int, size_t) = memset;
    volatile_memset(dst, val, sz);
}

template <typename T>
inline int internal_safe_memcmp(const T *a, const T *b, size_t size) {
    const unsigned char *a_ptr = (const unsigned char *)a;
    const unsigned char *b_ptr = (const unsigned char *)b;
    unsigned char result = 0;
    for (size_t i = 0; i < size; i++) {
        result |= a_ptr[i] ^ b_ptr[i];
    }
    return result;
}

template <typename T>
inline int internal_safe_free2(T **ptr, size_t size_in_byte) {
    if(ptr != NULL && *ptr != NULL) {
        internal_safe_memset(*ptr, 0, size_in_byte);
        free(*ptr);
        *ptr = NULL;
        return 1;
    }
    return 0;
}

inline int internal_safe_free(void *ptr, size_t size_in_byte) {
    if(ptr != NULL) {
        internal_safe_memset(ptr, 0, size_in_byte);
        free(ptr);
        return 1;
    }
    return 0;
}

inline std::string toLowerStr(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c){ return std::tolower(c); });
    return result;
}