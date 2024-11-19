
#include "stdint.h"
#include "stdio.h"
#include "print.h"
#include <stdarg.h>



#define PRINTF_STATE_NORMAL         0
#define PRINTF_STATE_LENGTH         1
#define PRINTF_STATE_LENGTH_SHORT   2
#define PRINTF_STATE_LENGTH_LONG    3
#define PRINTF_STATE_SPEC           4

#define PRINTF_LENGTH_DEFAULT       0
#define PRINTF_LENGTH_SHORT_SHORT   1
#define PRINTF_LENGTH_SHORT         2
#define PRINTF_LENGTH_LONG          3
#define PRINTF_LENGTH_LONG_LONG     4


void printf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int state = PRINTF_STATE_NORMAL;
    int length = PRINTF_LENGTH_DEFAULT;

    int radix = 10;
    bool sign = false;

    uint64_t paramNum = 2;


    while(*fmt) {
        switch(state) {
            case PRINTF_STATE_NORMAL:
                switch (*fmt)
                {
                    case '%':
                        state = PRINTF_STATE_LENGTH;
                        break;
                    default:
                        print_char(*fmt);
                        break;
                }
                break;
            case PRINTF_STATE_LENGTH:
                switch (*fmt)
                {
                    case 'h':   length = PRINTF_LENGTH_SHORT;
                                state = PRINTF_STATE_LENGTH_SHORT;
                                break;
                    case 'l':   length = PRINTF_LENGTH_LONG;
                                state = PRINTF_STATE_LENGTH_LONG;
                                break;
                    default:    goto PRINTF_STATE_SPEC_;
                }
                break;
            case PRINTF_STATE_LENGTH_SHORT:
                if(*fmt == 'h') {
                    length = PRINTF_LENGTH_SHORT_SHORT;
                    state = PRINTF_STATE_SPEC;
                }
                else goto PRINTF_STATE_SPEC_;
                break;
            case PRINTF_STATE_LENGTH_LONG:
                if(*fmt == 'l') {
                    length = PRINTF_LENGTH_LONG_LONG;
                    state = PRINTF_STATE_SPEC;
                }
                else goto PRINTF_STATE_SPEC_;
                break;
            case PRINTF_STATE_SPEC:
            PRINTF_STATE_SPEC_:
                switch (*fmt)
                {
                    case 'c':   print_char(va_arg(args, char));
                                paramNum++;
                                break;
                    case 's':   print_str(va_arg(args, char*));
                                paramNum++;
                                break;
                    case '%':   print_char('%');
                                break;
                    case 'd':
                    case 'i':   radix = 10; sign = true;
                                printf_number(va_arg(args, uint64_t), length, sign, radix);
                                paramNum++;
                                break;
                    case 'u':   radix = 10; sign = false;
                                printf_number(va_arg(args, uint64_t), length, sign, radix);
                                paramNum++;
                                break;
                    case 'X':
                    case 'x':
                    case 'p':   radix = 16; sign = false;
                                printf_number(va_arg(args, uint64_t), length, sign, radix);
                                paramNum++;
                                break;
                    case 'o':   radix = 8; sign = false;
                                printf_number(va_arg(args, uint64_t), length, sign, radix);
                                paramNum++;
                                break;
                    default: break; // ignore invalid specifiers
                }
                // reset state
                state = PRINTF_STATE_NORMAL;
                length = PRINTF_LENGTH_DEFAULT;
                radix = 10;
                sign = false;
                break;
        }
        fmt++;
    }
    va_end(args);
}

const char g_HexChars[] = "0123456789abcdef";

void printf_number(uint64_t value, int length, bool sign, int radix) {
    char buffer[32];
    unsigned long long number;
    int number_sign = 1;
    int pos = 0;

    switch (length)
    {
        case PRINTF_LENGTH_SHORT_SHORT:
        case PRINTF_LENGTH_SHORT:
        case PRINTF_LENGTH_DEFAULT:
            if(sign) {
                int n = value;
                if(n < 0) {
                    n = -n;
                    number_sign = -1;
                }
                number = (unsigned long long) n;
            } else {
                number = (unsigned int) value;
                number_sign = 1;
            }
            break;
        case PRINTF_LENGTH_LONG:
            if(sign) {
                long int n = (long int) value;
                if(n < 0) {
                    n = -n;
                    number_sign = -1;
                }
                number = (unsigned long long) n;
            } else {
                number = (unsigned long int) value;
                number_sign = 1;
            }
            break;
        case PRINTF_LENGTH_LONG_LONG:
            if(sign) {
                long long int n = (long long int) value;
                if(n < 0) {
                    n = -n;
                    number_sign = -1;
                }
                number = (unsigned long long) n;
            } else {
                number = (unsigned long long) value;
                number_sign = 1;
            }
            break;

    }
    
    do {
        uint32_t rem = number % radix;
        number /= radix;
        buffer[pos++] = g_HexChars[rem];
    } while(number > 0);

    if(sign && number_sign < 0) {
        buffer[pos++] = '-';
    }

    while(--pos >= 0) {
        print_char(buffer[pos]);
    }

}