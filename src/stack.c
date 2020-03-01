#include "../include/signal_handler.h"
#include "../include/stack.h"

#include <assert.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* Terminal colors:  */
#define RED "\x1B[31m"
#define RESET "\x1B[0m"

#define DEFAULT_CAPACITY 10
#define MAX_STACK_COUNT 1024

/* Check if stack is okey  */
#define ASSERT_OK();\
 if (is_corrupted(i_stack)) {\
    print_debug_info(stack, STACK_ERR);\
    assert(!is_corrupted(i_stack));\
}

/* Left and right confines of CANARY values  */
#define CANARY_LEFT_END(   stack) ( stack->capacity / 2 + (stack)->capacity % 2 )
#define CANARY_RIGHT_START(stack) ( stack->capacity * 2 - (stack)->capacity / 2 )
#define CANARY 0xDEADBEEF

#define BASE_INFO(stack) "top=%d, capacity=%d\n",\
           stack->top - stack->capacity / 2 - stack->capacity % 2,\
                                                             stack->capacity

/* Create private stack from public  */
#define CAST_INTERN(to, from); internal_stack *to \
                                          = (internal_stack *) from->internal;

/* Created on stack because of no need to free memory  */
static custom_stack *stacks[MAX_STACK_COUNT]; /** Contains created stacks */
static int current_stack_count = 0; /** Shows stacks count created */
static int errno = 0; /** error flags  */

/**
* @brief Stack with private fields.
*/
typedef struct {
    int top;
    int capacity;
    double *buffer;
} internal_stack;

custom_stack * create_custom_stack(int capacity) {
    int i;
    if (current_stack_count >= MAX_STACK_COUNT) {
        return NULL;
    }
    size_t internal_size = sizeof(internal_stack);
    custom_stack *stack = (custom_stack *) malloc(sizeof(custom_stack));
    stack->internal = (void *) malloc(internal_size);
    CAST_INTERN(i_stack, stack);
    
    i_stack->capacity = capacity;
    i_stack->top = CANARY_LEFT_END(i_stack);
    i_stack->buffer = (double *) malloc(2*capacity*sizeof(double)+capacity%2);
    for (i = 0; i < CANARY_LEFT_END(i_stack); i++) {
        i_stack->buffer[i] = CANARY;
    }
    for (i = CANARY_RIGHT_START(i_stack); i < 2 * capacity + capacity % 2; i++) {
        i_stack->buffer[i] = CANARY;
    }
    
    stacks[current_stack_count] = stack;
    if (current_stack_count == 0) {
        signal(SIGSEGV, signal_handler);
    }
    current_stack_count++;
    return stack;
}

/**
* @brief Check if stack is empty
* @param stack pointer to stack
* @return 1 if empty
*         0 else
*/
int is_empty(internal_stack *i_stack) {
    if (i_stack->top == CANARY_LEFT_END(i_stack)) {
        return 1;
    }
    return 0;
}

/**
* @brief Check if stack is full
* @param stack pointer to stack
* @return 1 if full
*         0 else
*/
int is_full(internal_stack *i_stack) {
    if (i_stack->top == CANARY_RIGHT_START(i_stack)) {
        return 1;
    }
    return 0;
}

/**
* @brief Check canaries.
* @param i_stack - target stack pointer
* @return 1 if canaries failed
*         0 else
*/
int fail_canaries(internal_stack *i_stack) {
    int i = 0;
    for (i = 0; i < CANARY_LEFT_END(i_stack); i++) {
        if (i_stack->buffer[i] != CANARY) {
            return 1;
        }
    }
    for (i = CANARY_RIGHT_START(i_stack); i < 2 * i_stack->capacity 
                                               + i_stack->capacity % 2; i++) {
        if (i_stack->buffer[i] != CANARY) {
              return 1;
          }
    }
    return 0;
}

/**
* @brief Check if stack was corrupted and write errno flags.
* @param i_stack - target stack pointer
* @return errno - error flags
* @note if i_stack == NULL then we must go out from function
*/
int is_corrupted(internal_stack *i_stack) {
    if (i_stack == NULL) {
        errno = 0b1000;
        return errno;
    }
    if (i_stack->top < CANARY_LEFT_END(i_stack)) {
        errno = 0b0100;
    }
    if (i_stack->top > CANARY_RIGHT_START(i_stack)) {
        errno |= 0b0010;
    }
    if (fail_canaries(i_stack)) {
        errno |= 0b0001;
    }
    return errno;
}

/**
* @note We should divide cases when stack is full, empty or corrupted
*/
void push(custom_stack *stack, double elem) {
    CAST_INTERN(i_stack, stack);
    //printf("top %d\n", i_stack->top);
    ASSERT_OK();
    if (is_full(i_stack)) {
      return;
    }
    (i_stack->buffer)[i_stack->top] = elem;
    (i_stack->top)++;
    // For canary error
    //i_stack->buffer[0] = 1488;
    ASSERT_OK();
}

void pop(custom_stack *stack) {
    CAST_INTERN(i_stack, stack);
    ASSERT_OK();
    if (is_empty(i_stack)) {
        return;
    }
    (i_stack->top)--;
    (i_stack->buffer)[i_stack->top] = 0;
    ASSERT_OK();
}

double top(custom_stack *stack) {
    CAST_INTERN(i_stack, stack);
    ASSERT_OK();
    return i_stack->buffer[i_stack->top - 1];
}

void print_stack_elements(internal_stack *i_stack) {
    int i;
    printf("elements:\n");
    for (i = CANARY_LEFT_END(i_stack); i < CANARY_RIGHT_START(i_stack) - 1; i++) {
        printf("%f (%p), ", i_stack->buffer[i], &(i_stack->buffer[i]));
    }
    printf("%f (%p)\n", i_stack->buffer[i], &(i_stack->buffer[i]));
}

void print_debug_info(custom_stack *stack, int MODE) {
    CAST_INTERN(i_stack, stack);
    printf("******************************\n");
    switch (MODE) {
        case STACK_INFO:
            printf(BASE_INFO(i_stack));
            print_stack_elements(i_stack);
            break;
        case STACK_ERR:
            printf(BASE_INFO(i_stack));
            print_stack_elements(i_stack);
            printf("Stack pointer: %p\n", (void *) stack);
            if (errno != 0) {
                if (errno & 0b1000) {
                    printf(RED "[STACK IS NULL]\n" RESET);
                }
                if (errno & 0b0100) {
                    printf(RED "[TOP IS MORE LEFT THAN POSSIBLE\n" RESET);
                }
                if (errno & 0b0010) {
                    printf(RED "[TOP IS MORE RIGHT THAN POSSIBLE]\n" RESET);
                }
                if (errno & 0b0001) {
                    printf(RED "[CANARY CHECK FAILED]\n" RESET);
                }
                errno = 0;
            }
            break;
        default:
            printf("No output mode specified\n");
            break;
    }
    /* If we want SIGSEGV */
    //printf("%d\n", i_stack->buffer[100025]);
    printf("******************************\n");
}

void dump_all_stacks() {
    int i;
    for (i = 0; i < current_stack_count; i++) {
        print_debug_info(stacks[i], STACK_ERR);
    }
}

void free_buffer(custom_stack *stack) {
    CAST_INTERN(i_stack, stack);
    free(i_stack->buffer);
}
