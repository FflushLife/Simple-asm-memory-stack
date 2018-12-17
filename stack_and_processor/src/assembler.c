#include "../include/assembler.h"
#include "../include/stack.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

#define DEFINE_NEW_ASM_COMMAND(name) {ASM_##name, #name, asm_##name##_func}
#define EXE_ASM() void (*f)()=asm_commands[regs.r0].func; (*f)();
// Here we get register currently loaded into exe command
#define REG (*(&(regs.ax) + regs.r1))
#define SIDE_REG (*(&(regs.ax) + regs.r2))

#define BUF_LEN 255
#define CMD_BUF_LEN 2056
#define MAGIC 534523679


static custom_stack *stack;
static custom_stack *rets;

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
static int saved_cmd_pointer = 0;
static int tag_pointer = 0;
static int last_tag = 0;
static int cmds[CMD_BUF_LEN];
static int tags[CMD_BUF_LEN];
static char tags_names[CMD_BUF_LEN][CMD_BUF_LEN];
static char strings[4][255] = {"There is 0 solutions\n", "There is 1 solution\n", "There is 2 solutions\n",
    "There is infinity solutions\n"};


int get_tag_by_name(const char *tag) {
    int i = 0;
    while(i < CMD_BUF_LEN) {
        if (!strcmp(tags_names[i], tag)) {
            return i;
        }
        i++;
    }
    return -1;
}

// One genius hack is here: we get registers by pointer to raw memory
void asm_add_func() {
    if (regs.r3 == 0) {
        REG += SIDE_REG;
        return;
    }
    REG += regs.r2;
}

void asm_sub_func() {
    if (regs.r3 == 0) {
        REG -= SIDE_REG;
        return;
    }
    REG -= regs.r2;
}

void asm_mul_func() {
    if (regs.r3 == 0) {
        REG *= SIDE_REG;
        return;
    }
    REG *= regs.r2;
}

void asm_div_func() {
    if (regs.r3 == 0) {
        REG /= SIDE_REG;
        return;
    }
    REG /= regs.r2;
}

void asm_mov_func() {
    /*
       Implement modes for r3 firstly for
       addressing by register or value or address
    */
}

void asm_push_func() {
    if (regs.r3 == 1) {
        push(stack, REG);
        return;
    }
    push(stack, regs.r1);
}

void asm_pop_func() {
    regs.dx = top(stack);
    pop(stack);
}

void asm_call_func() {
    //saved_cmd_pointer = cmd_pointer;
    push(rets, cmd_pointer);
    cmd_pointer = regs.r1;
}

void asm_ret_func() {
   cmd_pointer = (int) top(rets);
   pop(rets); 
}

void asm_sqrt_func() {
    REG = sqrt(REG);
}

void asm_je_func() {
   if (regs.ax == regs.bx) {
       push(rets, cmd_pointer);
       cmd_pointer = regs.r1;
   } 
}

void asm_jne_func() {
   if (regs.ax != regs.bx) {
       push(rets, cmd_pointer);
       cmd_pointer = regs.r1;
   } 
}

void asm_in_func() {
    int val = 0;
    scanf("%d", &val);
    REG = val;
}

void asm_fluffy_bastard_func() {
    printf("\n\n\n\n\n(^._.^)ﾉ☆( _ _).oO\n\n\n\n\n");
}

void asm_cmp_func() {
   // Implement this 
}

void asm_out_func() {
    printf("%s\n", strings[regs.r1]);
}

struct asm_command asm_commands[] = {
    DEFINE_NEW_ASM_COMMAND(add),
    DEFINE_NEW_ASM_COMMAND(sub),
    DEFINE_NEW_ASM_COMMAND(mul),
    DEFINE_NEW_ASM_COMMAND(div),
    DEFINE_NEW_ASM_COMMAND(mov),
    DEFINE_NEW_ASM_COMMAND(push),
    DEFINE_NEW_ASM_COMMAND(pop),
    DEFINE_NEW_ASM_COMMAND(call),
    DEFINE_NEW_ASM_COMMAND(ret),
    DEFINE_NEW_ASM_COMMAND(sqrt),
    DEFINE_NEW_ASM_COMMAND(je),
    DEFINE_NEW_ASM_COMMAND(jne),
    DEFINE_NEW_ASM_COMMAND(in),
    DEFINE_NEW_ASM_COMMAND(fluffy_bastard),
    DEFINE_NEW_ASM_COMMAND(cmp),
    DEFINE_NEW_ASM_COMMAND(out)
}; 

const char commands_filename[] = "/media/data/home/m0p3d/Documents/MIPT_system_programming/stack/include/commands.txt";

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
    rets = create_custom_stack(20);
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
    fclose(file);
    return MAGIC;
}

int parse_arg(const char *arg) {
    // Set mode that means we have reg as the 2nd arg
    if (!strcmp(arg, "ax") || !strcmp(arg, "bx") || !strcmp(arg, "cx")
    || !strcmp(arg, "dx")) {
        if (regs.r1 != MAGIC && regs.r0 != ASM_push) { // If we have a reg as the 2nd arg
            regs.r3 = 0;
        } else if (regs.r0 == ASM_push && regs.r1 == MAGIC) { // If we have a reg in push arg
            regs.r3 = 1;
        }
    }
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
        if (regs.r0 == ASM_call || regs.r0 == ASM_je
        || regs.r0 == ASM_jne) { // This is call command
            regs.r1 = tags[get_tag_by_name(buf)];
        } else {
            regs.r1 = parse_arg(buf);
        }
    } else if (regs.r2 == MAGIC) {
        regs.r2 = parse_arg(buf);
    } else if (regs.r3 == MAGIC) {
        regs.r3 = parse_arg(buf); 
    }
}

int add_tag_from_buf(const char *buf) {
    strcpy(tags_names[last_tag++], buf);
    return 0;
}

// Get command with args from line
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

// Execute all asm commands from "byte code" step by step
int exe_all() {
    cmd_pointer = tags[get_tag_by_name("main")];
    printf("CMD POINTER: %d\n", cmd_pointer);
    while (cmds[cmd_pointer] != MAGIC) {
        fill_regs_from_cmds();
        EXE_ASM();
        print_regs(); 
    }
    return 0;
}

/*
 @brief run assembler code
 @param filename - path to asm program
*/
int parse_program(const char *filename) {
    FILE *file = fopen(filename, "r");
    char buf[BUF_LEN] = "";
    while(fgets(buf, BUF_LEN, file)) {
        if (!strcmp(buf, "\n")) {
            continue;
        }
        parse_command(buf);
        flush_regs();
    }
    fclose(file);
    print_debug_info(stack, 0);
    print_cmd_buf();
    print_tags();
    exe_all();
    print_debug_info(stack, 0);
    return 0;
}
