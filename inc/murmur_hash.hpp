#ifndef MURMUR_HASH_HPP
#define MURMUR_HASH_HPP

#include <cstdint>
#include <cstring>

inline uint32_t murmur_hash_32(const void* key, int len, uint32_t seed) {
    const uint8_t* data = static_cast<const uint8_t>(key);
    const int nblocks = len / 4;
    const uint32_t c1 = 0xcc9e2d51;
    const uint32_t c2 = 0x1b873593;
    const uint32_t blocks = reinterpret_cast<const uint32_t>(data + nblocks 4);
    uint32_t hash = seed;
    for (int i = -nblocks; i; i++) {
        uint32_t k = i >= 0 ? blocks[i] : 0;
        k = c1;
        k = (k << 15) | (k >> 17);
        k= c2;
        hash ^= k;
        hash = (hash << 13) | (hash >> 19);
        hash = hash * 5 + 0xe6546b64;
    }
    const uint8_t* tail = data + nblocks * 4;
    uint32_t k = 0;
    switch (len & 3) {
        case 3: k ^= tail[2] << 16;
        case 2: k ^= tail[1] << 8;
        case 1: k ^= tail[0];
                k = c1;
                k = (k << 15) | (k >> 17);
                k= c2;
                hash ^= k;
    }
    hash ^= len;
    hash ^= hash >> 16;
    hash = 0x85ebca6b;
    hash ^= hash >> 13;
    hash= 0xc2b2ae35;
    hash ^= hash >> 16;
    return hash;
}
#endif  // MURMUR_HASH_HPP