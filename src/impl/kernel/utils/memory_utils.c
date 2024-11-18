#include "memory_utils.h"
#include "stdint.h"

void* memcpy(void* dst, const void* src, uint16_t num)
{
    uint8_t* u8_dst = (uint8_t*)dst;
    const uint8_t* u8_src = (const uint8_t*)src;

    for (uint16_t i = 0; i < num; i++)
        u8_dst[i] = u8_src[i];

    return dst;
}

void* memset(void* ptr, int value, uint16_t num)
{
    uint8_t* u8_ptr = (uint8_t*)ptr;

    for (uint16_t i = 0; i < num; i++)
        u8_ptr[i] = (uint8_t)value;

    return ptr;
}

int memcmp(const void* ptr1, const void* ptr2, uint16_t num)
{
    const uint8_t* u8_ptr1 = (const uint8_t*)ptr1;
    const uint8_t* u8_ptr2 = (const uint8_t*)ptr2;

    for (uint16_t i = 0; i < num; i++)
        if (u8_ptr1[i] != u8_ptr2[i])
            return 1;

    return 0;
}