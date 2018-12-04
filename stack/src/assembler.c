#include "../include/assembler.h"
#include "../include/stack.h"

#include <stdio.h>
#include <string.h>

#define add(reg, val) (regs.reg += val)
#define DEFINE_NEW_ASM_COMMAND(name) {ASM_##name, #name, asm_##name##_func}
#define EXE_ASM() void (*f)()=asm_commands[regs.r0].func; (*f)();
// Here we get register currently loaded into exe command
#define REG (*(&(regs.ax) + regs.r1))

#define BUF_LEN 255
#define CMD_BUF_LEN 2056
#define MAGIC 534523679


static custom_stack *stack;
static struct registers regs = {
    .ax = 0.0,
    .bx = 0.0,
    .cx = 0.0,
    .dx = 0.0,
    .r0 = MAGIC,
    .r1 = MAGIC,
    .r2 = MAGIC,
    .r3 = MAGIC
};
static int cmd_pointer = 0;
static int tag_pointer = 0;
static int last_tag = 0;
static int cmds[CMD_BUF_LEN];
static int tags[CMD_BUF_LEN];
static char tags_names[CMD_BUF_LEN][CMD_BUF_LEN];


// One genius hack is here: we get registers by pointer to raw memory
void asm_add_func() {
    REG += regs.r2;
}

void asm_sub_func() {
    REG -= regs.r2;
}

void asm_mul_func() {
    REG *= regs.r2;
}

void asm_div_func() {
    REG /= regs.r2;
}

void asm_mov_func() {
    /*
       Implement modes for r3 firstly for
       addressing by register or value or address
    */
}

void asm_push_func() {
    push(stack, regs.r1);
}

void asm_pop_func() {
    regs.dx = top(stack);
    pop(stack);
}

void asm_call_func() {
    
}

void asm_ret_func() {
    
}

struct asm_command asm_commands[] = {
    DEFINE_NEW_ASM_COMMAND(add),
    DEFINE_NEW_ASM_COMMAND(sub),
    DEFINE_NEW_ASM_COMMAND(mul),
    DEFINE_NEW_ASM_COMMAND(sub),
    DEFINE_NEW_ASM_COMMAND(mov),
    DEFINE_NEW_ASM_COMMAND(push),
    DEFINE_NEW_ASM_COMMAND(pop),
    DEFINE_NEW_ASM_COMMAND(call),
    DEFINE_NEW_ASM_COMMAND(ret)
}; 

const char commands_filename[] = "/media/data/home/m0p3d/Documents/MIPT_system_programming/stack/include/commands.txt";

int get_tag_by_name(char *tag) {
    int i = 0;
    while(i < CMD_BUF_LEN) {
        if (!strcmp(tags_names[i], tag)) {
            return i;
        }
        i++;
    }
    return -1;
}

void flush_regs() {
    regs.ax = 0.0;
    regs.bx = 0.0;
    regs.cx = 0.0;
    regs.dx = 0.0;
    regs.r0 = MAGIC;
    regs.r1 = MAGIC;
    regs.r2 = MAGIC;
    regs.r3 = MAGIC;
}

void print_regs() {
    printf("Registers:\n ax: %f, bx: %f, cx: %f, dx: %f\n r0: %d, r1: %d, r2: %d, r3: %d\n",
    regs.ax, regs.bx, regs.cx, regs.dx, regs.r0, regs.r1, regs.r2, regs.r3);
}

void print_cmd_buf() {
    int i;
    printf("Commands in line: \n");
    for (i = 0; i < CMD_BUF_LEN; i++) {
        if (cmds[i] == MAGIC && i % 4 == 0) {
            printf("\n");
            return;
        }
        printf("%d ", cmds[i]);
    }
}

void print_tags() {
    int i = 0;
    printf("Tags:\n");
    for (; i < last_tag; i++) {
        printf("%s\n", tags_names[i]);
    }
} 

int init_asm() {
    int i;
    stack = create_custom_stack(20);
    for (i = 0; i < CMD_BUF_LEN; i++) {
        cmds[i] = MAGIC;
    }
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
            return command_index;
        }
        memset(command, '\0', BUF_LEN);
    }
    return MAGIC;
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

void fill_cmd_buf() {
    cmds[cmd_pointer++] = regs.r0;
    cmds[cmd_pointer++] = regs.r1;
    cmds[cmd_pointer++] = regs.r2;
    cmds[cmd_pointer++] = regs.r3;
}

void fill_regs_from_cmds() {
    regs.r0 = cmds[cmd_pointer++];
    regs.r1 = cmds[cmd_pointer++];
    regs.r2 = cmds[cmd_pointer++];
    regs.r3 = cmds[cmd_pointer++];
}

void fill_reg(const char *buf) {
    if (regs.r0 == MAGIC) {
        regs.r0 = get_cmd_by_name(buf);
    } else if (regs.r1 == MAGIC) {
        regs.r1 = parse_arg(buf);
    } else if (regs.r2 == MAGIC) {
        regs.r2 = parse_arg(buf);
    } else if (regs.r3 == MAGIC) {
        regs.r3 = parse_arg(buf); 
    }
}

int add_tag_from_buf(char *buf) {
    strcpy(tags_names[last_tag++], buf);
    return 0;
}

int parse_command(const char *line) {
    char buf[BUF_LEN] = "";
    int i = 0;
    int buf_i = 0;
    char symb = line[0];
    while (symb) {
        if (symb == ' ' || symb == ',') {
            if (buf_i == 0) {
                goto next_symb;
            }
            fill_reg(buf);
            memset(buf, '\0', BUF_LEN);
            buf_i = 0;
        } else if (symb == ':') {
            tags[tag_pointer++] = cmd_pointer;
            add_tag_from_buf(buf);
            memset(buf, '\0', BUF_LEN);
            goto exit;
        } else if (symb != 10) { //Decline new line symbol
            buf[buf_i++] = symb;
        }
next_symb:
        symb = line[++i];
    }
    fill_reg(buf);
    // For correct output
    if (regs.r3 == MAGIC) {
        regs.r3 = -1;
    }
    fill_cmd_buf();
exit:
    return 0;
}

void exe_all() {
    cmd_pointer = 0;
    while (cmds[cmd_pointer] != MAGIC) {
        fill_regs_from_cmds();
        EXE_ASM();
        print_regs(); 
    }
}

int parse_program(const char *filename) {
    FILE *file = fopen(filename, "r");
    char buf[BUF_LEN] = "";
    while(fgets(buf, BUF_LEN, file)) {
        parse_command(buf);
        flush_regs();
    }
    print_debug_info(stack, 0);
    print_cmd_buf();
    print_tags();
    exe_all();
    return 0;
}
