#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include "internal_utilities.hpp"

const uint32_t encrypted_initial[8] = {0xcdbc21b2, 0x1cd26950, 0x9bdb34a7, 0x02fa32ef, 0xf6bb95aa, 0x3cb0af59, 0xb8361e7e, 0xfc550acc};
const uint32_t encrypted_k[64] = {
    0x8f360e2a, 0x6de52dc1, 0x2e1bcf68, 0xeb4fe94a, 0xcfed57f1, 0x6541bea8, 0x2a099cda, 0x57495419,
    0x2452a054, 0xaab5457f, 0x18812ae7, 0xa3b7e869, 0x70446f9b, 0x1b058559, 0x870e6ff7, 0x0c27d0c6,
    0x29274873, 0xf36c2ed6, 0x941aa961, 0x26f69323, 0xdb52b9c5, 0x76c42bf3, 0xe486b7a2, 0x8aac8216,
    0x646b5b9e, 0x1007d813, 0x8cb38891, 0x49e2ea6d, 0xc41a391c, 0x4e7ca5e0, 0x1a180a01, 0xd99508d5,
    0xea0b2b37, 0x32c94868, 0xd6f7595b, 0x51c23ffc, 0x93b1e6fe, 0x4adaa5e2, 0x39f4d750, 0x6e272649,
    0x5eeae26d, 0x102c7835, 0xfefb2429, 0x31d7c409, 0xd368daf6, 0x4d423283, 0xe8dc5cd5, 0xddd681c2,
    0xd418e0a4, 0x02e50558, 0xbc9343eb, 0x364a8e5a, 0xcfa79919, 0x72680513, 0xe3aad431, 0x947b653f,
    0x88da8822, 0xc0937d11, 0xb878d74d, 0x7a7c97a2, 0x9244cd15, 0x3f8b584c, 0xa22bcaa7, 0x0bcd5940
};

std::string sha256(const std::string&);


//TODO temporary, building is as fast as possible

uint32_t swap_byte_endian_32(uint32_t x) {
    uint32_t tmp = (x & 255) << 24;
    tmp |= (((x >> 8) & 255) << 16);
    tmp |= (((x >> 16) & 255) << 8);
    tmp |= ((x >> 24) & 255);
    return tmp;
}

uint32_t rotr32(uint32_t c, uint8_t n) {
    return (c >> n) | (c << (32-n));
}

void decrypt_constants(uint32_t decrypted[72], uint32_t **initial, uint32_t **k) {
    for(int i=0; i < 8; i++) decrypted[i] = encrypted_initial[i] ^ 0xA7B5C7D5u;
    for (int i = 0; i < 64; i++) {
        int index;
        if ((i / 8) % 2 == 0) {
            index = i % 8;
        } else {
            index = 7 - (i % 8);
        }
        decrypted[i+8] = encrypted_k[i] ^ encrypted_initial[index];
    }
    *initial = &decrypted[0];
    *k = &decrypted[8];
}

void sha256_impl(uint32_t arr[16], uint32_t state[8]) {
    uint32_t decrypted[72];
    uint32_t *initial, *k;
    decrypt_constants(decrypted, &initial, &k);

    uint32_t *h = (uint32_t*) malloc(32);
    memcpy(h, state, 32);
    uint32_t *w = (uint32_t*) malloc(256);
    memcpy(w, arr, 64);
    for(int i=16; i < 64; i++) {
        uint32_t s0 = rotr32(w[i-15], 7) ^ rotr32(w[i-15], 18) ^ (w[i-15] >> 3);
        uint32_t s1 = rotr32(w[i-2], 17) ^ rotr32(w[i-2], 19) ^ (w[i-2] >> 10);
        w[i] = s0 + s1 + w[i-7] + w[i-16];
    }

    for(int i=0; i < 64; i++) {
        uint32_t sum0, sum1, temp1, temp2, maj, ch;
        sum0 = rotr32(h[0], 2) ^ rotr32(h[0], 13) ^ rotr32(h[0], 22);
        sum1 = rotr32(h[4], 6) ^ rotr32(h[4], 11) ^ rotr32(h[4], 25);
        maj = (h[0] & h[1]) ^ (h[0] & h[2]) ^ (h[1] & h[2]);
        ch = (h[4] & h[5]) ^ ((~h[4]) & h[6]);
        temp1 = sum1 + ch + k[i] + w[i] + h[7];
        temp2 = sum0 + maj;

        memmove(h+1, h, 28);
        h[0] = temp1 + temp2;
        h[4] += temp1;
    }
    for(int i=0; i < 8; i++) state[i] += h[i];

    internal_safe_free2(&h, 32);
    internal_safe_free2(&w, 256);
}

//Byte version for sha256 only support at most (2^64)/8 bytes
void sha256_bytes(const char *input_arr, uint64_t no_of_bytes, const uint32_t initial_state[8], uint32_t output_state[8]) {
    uint32_t *state = (uint32_t*) malloc(32);
    memcpy(state, initial_state, 32);
    uint64_t no_of_bits = no_of_bytes*8;
    uint64_t aligned_bits = ((no_of_bits+65)/512 + ((no_of_bits+65) % 512 != 0))*512;
    uint64_t aligned_dwords = aligned_bits/32;
    uint32_t *arr = (uint32_t*) calloc(aligned_dwords, 4);
    memcpy(arr, input_arr, no_of_bytes);
    arr[no_of_bytes/4] |= ((uint32_t) 0x80 << 8*(no_of_bytes % 4));
    for(uint64_t i=0; i < aligned_dwords; i++) arr[i] = swap_byte_endian_32(arr[i]);
    arr[aligned_dwords-2] = (uint32_t) (no_of_bits >> 32);
    arr[aligned_dwords-1] = (uint32_t) no_of_bits;

    for(uint64_t i=0; i < aligned_dwords; i+=16) sha256_impl(&arr[i], state);

    memcpy(output_state, state, 32);

    internal_safe_free2(&arr, aligned_dwords*4);
    internal_safe_free2(&state, 32);
}

std::string sha256(const std::string& input_arr) {
    uint32_t decrypted[72];
    uint32_t *initial, *k;
    decrypt_constants(decrypted, &initial, &k);

    uint32_t *state = (uint32_t*) malloc(32);
    sha256_bytes(input_arr.c_str(), input_arr.size(), initial, state);
    char retstr[65] = {0};
    char tstr[10];
    for(int i=0; i < 8; i++) {
        internal_safe_memset(tstr, 0, sizeof(tstr));
        snprintf(tstr, 9, "%.8x", state[i]);
        strcat(retstr, tstr);
        internal_safe_memset(tstr, 0, sizeof(tstr));
    }

    internal_safe_free2(&state, 32);
    return std::string(retstr);
}