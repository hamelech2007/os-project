#include "string.h"
#include "stdint.h"


uint16_t strlen(char* str) {
    uint16_t len = 0;
    while(*str) {
        len++;
        str++;
    }

    return len;
}

uint8_t strcmp(char* str1, char* str2) {
    uint8_t i;
    while(*str1 && *str2) {
        if(i = (*str2  - *str1)) {
            return i;
        }
        str1++;
        str2++;
    }
    return !(*str1 || *str2);
}