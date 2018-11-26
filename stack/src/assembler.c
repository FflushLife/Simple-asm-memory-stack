#include "../include/assembler.h"
#include "../include/stack.h"

#include <stdio.h>
#include <string.h>

#define add(reg, val) (regs.reg += val)
#define DEFINE_NEW_ASM_COMMAND(name) {ASM_##name, #name, asm_##name##_func}
#define EXE_ASM() void (*f)()=asm_commands[regs.ax].func; (*f)();

#define BUF_LEN 255
#define MAGIC 534523679

void asm_add_func() {
   printf("Hello from add func\n"); 
}

void asm_sub_func() {
    printf("Hello from sub func\n");
}

void asm_mul_func() {
    printf("Hello from mul func\n");
}

static custom_stack *stack;
static struct registers regs = {
    .ax = MAGIC,
    .bx = MAGIC,
    .cx = MAGIC,
    .dx = MAGIC
};

struct asm_command asm_commands[] = {
    DEFINE_NEW_ASM_COMMAND(add),
    DEFINE_NEW_ASM_COMMAND(sub),
    DEFINE_NEW_ASM_COMMAND(mul)
}; 

const char commands_filename[] = "/media/data/home/m0p3d/Documents/MIPT_system_programming/stack/include/commands.txt";

void flush_regs() {
    regs.ax = MAGIC;
    regs.bx = MAGIC;
    regs.cx = MAGIC;
    regs.dx = MAGIC;
}

void print_regs() {
    printf("Registers:\n ax: %d, bx: %d, cx: %d, dx:%d\n", regs.ax, regs.bx,
    regs.cx, regs.dx);
}

int cmd(const char *command, size_t len) {
    if (!strcmp(command, "add")) {
        add(bx, 1);
    }
    return 0;
}

int init_asm() {
    stack = create_custom_stack(20);
    return 0;
}

int get_cmd_from_line(const char *line, char *ret_name) {
    char buf[BUF_LEN] = "";
    char symb = line[0];
    int i = 0;
    int buf_i = 0;
    while (symb) {
       if (symb == ' ') {
           strncpy(ret_name, buf, BUF_LEN);
           memset(buf, '\0', BUF_LEN);
           buf_i = 0;
       } else {
           buf[buf_i++] = symb;
       }
       symb = line[++i];
    }
    return atoi(buf);
}

int get_cmd_by_name(const char *cmd_name) {
    FILE *file = fopen(commands_filename, "r");
    char buf[BUF_LEN] = "";
    char command[BUF_LEN] = "";
    int command_index = -1;
    while(fgets(buf, BUF_LEN, file)) {
        command_index = get_cmd_from_line(buf, command);
        if (!strcmp(command, cmd_name)) {
            regs.ax = command_index;
            break;
        }
        memset(command, '\0', BUF_LEN);
    }
    return 0;
}

int parse_arg(const char *arg) {
    if (!strcmp(arg, "ax")) {
        return 0;
    } else if (!strcmp(arg, "bx")) {
        return 1;
    } else if (!strcmp(arg, "cx")) {
        return 2;
    } else if (!strcmp(arg, "dx")) {
        return 3;
    } else {
        return atoi(arg);
    }
}

void fill_reg(const char *buf) {
    int command = -1;
    if (regs.ax == MAGIC) {
        command = get_cmd_by_name(buf);
    } else if (regs.bx == MAGIC) {
        regs.bx = parse_arg(buf);
    } else if (regs.cx == MAGIC) {
        regs.cx = parse_arg(buf);
    } else if (regs.dx == MAGIC) {
        regs.dx = parse_arg(buf); 
    }
}

void save_regs() {
    push(stack, regs.ax);
    push(stack, regs.bx);
    push(stack, regs.cx);
    push(stack, regs.dx);
}

void load_regs() {
    regs.dx = top(stack);
    pop(stack);
    regs.cx = top(stack);
    pop(stack);
    regs.bx = top(stack);
    pop(stack);
    regs.ax = top(stack);
    pop(stack);
}

int parse_command(const char *line) {
    char buf[BUF_LEN] = "";
    int i = 0;
    int buf_i = 0;
    char symb = line[0];
    while(symb) {
        if (symb == ' ' || symb == ',') {
            if (buf_i == 0) {
                goto next_symb;
            }
            fill_reg(buf);
            memset(buf, '\0', BUF_LEN);
            buf_i = 0;
        } else if (symb != 10) { //Decline new line symbol
            buf[buf_i++] = symb;
        }
next_symb:
        symb = line[++i];
    }
    fill_reg(buf);
    EXE_ASM();
    return 0;
}

int parse_program(const char *filename) {
     FILE *file = fopen(filename, "r");
     char buf[BUF_LEN] = "";
     while(fgets(buf, BUF_LEN, file)) {
         save_regs();
         parse_command(buf);
         print_regs();
         load_regs();
         print_regs();
         flush_regs();
     }
     return 0;
}
