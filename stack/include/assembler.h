#include "stdlib.h"

struct registers {
    int ax;
    int bx;
    int cx;
    int dx;
};

struct asm_command {
    int number;
    char *name;
    void *func;
};

enum commands {
    ASM_add,
    ASM_sub,
    ASM_mul,
    ASM_div,
    ASM_jmp
};

int cmd(const char *, size_t);
int init_asm();
void print_regs();
int parse_program(const char *);
