#include "print.h"
#include "stdio.h"
#include "string.h"
#include "memory.h"
#include <stddef.h>

const static size_t NUM_COLS = 80;
const static size_t NUM_ROWS = 25;
const static size_t PREFIX_LEN = 9;

struct vgaPoint {
    uint8_t row, col;
};
struct vgaPoint deletePoint = {0, 0};

struct Char {
    uint8_t character;
    uint8_t color;
};

struct Char* buffer = (struct Char*) 0xb8000;
size_t col = 0;
size_t row = 0;
uint8_t color = PRINT_COLOR_WHITE | (PRINT_COLOR_BLACK << 4);

static char* commands[] = {"uptime", 0};

extern uint64_t startup_timer;
void parse_command() {
    struct Char* commandLine = buffer + row*NUM_COLS + PREFIX_LEN;
    char** currCommands = commands;
    while(*currCommands) {
        // todo fix
        char* command = *currCommands;
        uint16_t index = 0;
        while(*(command + index)){
            if(index + PREFIX_LEN >= col) break;
            if(*(command + index) == (commandLine + index)->character) {
                index++;
            } else break;
        }
        if(index == strlen(command) && (commandLine + index)->character == ' ') {
            // command found
            if(strcmp(command, "uptime")) {
                uint64_t uptime = startup_timer*59.9;
                print_char('\n');
                print_str("The OS has been up for ");
                print_int((uptime/1000)/60/60 % 60);
                print_str(" hours, ");
                print_int((uptime/1000)/60 % 60);
                print_str(" minutes and ");
                print_int((uptime/1000) % 60);
                print_str(" seconds!\n");
                return;
            }
            return;
        }
        currCommands++;
    }
    print_char('\n');
    print_str("Command not found!\n");
}

void clear_row(size_t row) {
    struct Char empty = (struct Char) {
        character: ' ',
        color: color
    };

    for(size_t col = 0; col < NUM_COLS; col++) {
        buffer[col + NUM_COLS * row] = empty;
    }
}


void print_newline() {
    col = 0;

    if(row < NUM_ROWS - 1) {
        row++;
        return;
    }

    for(size_t row_i = 1; row_i < NUM_ROWS; row_i++) {
        for(size_t col_i = 0; col_i < NUM_COLS; col_i++) {
            struct Char character = buffer[col_i + NUM_COLS * row_i];
            buffer[col_i + NUM_COLS * (row_i - 1)] = character;
        }
    }
    clear_row(NUM_ROWS - 1);
}

void print_clear() {
    for(size_t i = 0; i < NUM_ROWS; i++) {
        clear_row(i);
    }
}

void print_char(char character) {
    if(character == '\n') {
        print_newline();
        return;
    }

    if(col > NUM_COLS) {
        print_newline();
    }

    buffer[col + NUM_COLS * row] = (struct Char) {
        character: (uint8_t) character,
        color: color
    };

    col++;
}

void print_str(char* string) {
    for(size_t i = 0; 1; i++){
        char character = (uint8_t) string[i];

        if(character == 0) return;

        print_char(character); 
    }
}
char* hexChars = "0123456789abcdef";
void print_hex(uint64_t num) {
    char buffer[32];
    buffer[31] = 0;
    int i = 30;
    do {
        buffer[i--] = hexChars[num%16];
        num/=16;
    } while(num != 0);

    print_str(buffer + i + 1);
}

void print_int(uint64_t num) {
    char buffer[32];
    buffer[31] = 0;
    int i = 30;
    do {
        buffer[i--] = num%10+'0';
        num/=10;
    } while(num != 0);

    print_str(buffer + i + 1);

}

void print_set_color(uint8_t foreground, uint8_t background) {
    color = foreground | (background << 4);
}

void delete_char() {
    if(col <= deletePoint.col && row <= deletePoint.row) return;
    struct Char empty = (struct Char) {
        character: ' ',
        color: color
    };

    if(col == 0 && row == 0) return;
    if(col == 0){
        col = NUM_COLS;
        row--;
    } else col--;
    buffer[col + NUM_COLS * row] = empty;
}

char peek_char() {
    return buffer[col + NUM_COLS * row].character;
}

void disable_deletion() {
    deletePoint = (struct vgaPoint) {row, col};
}
void enable_deletion() {
    deletePoint = (struct vgaPoint) {0, 0};
}

void print_prefix() {
    print_str("command: ");
}