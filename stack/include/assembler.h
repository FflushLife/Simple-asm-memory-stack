#include "stdlib.h"

struct registers {
    double ax;
    double bx;
    double cx;
    double dx;
    int r0;
    int r1;
    int r2;
    int r3;
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
    ASM_mov,
    ASM_push,
    ASM_pop,
    ASM_call,
    ASM_ret,
    ASM_sqrt
};

int init_asm();
void print_regs();
int parse_program(const char *);
