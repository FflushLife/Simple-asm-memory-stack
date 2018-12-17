#include "../include/stack.h"

#include <stdio.h>
#include <stdlib.h>


int main() {
    int i;
    custom_stack *stack = create_custom_stack(11);
    printf("Stack info: \n");
    print_debug_info(stack, STACK_INFO);
    for (i = 0; i < 12; i++) {
        push(stack, i);
    }
    pop(stack);
    printf("Stack info: \n");
    print_debug_info(stack, STACK_INFO);

    /* Stack corruption (shifting top to right) */
    //*(int*)(stack->internal)) = 21421;

    /* Stack corruption (call SIGSEGV) */
    //*(int*)(stack->internal + 2 * sizeof(int)) = 21421;
    pop(stack);
    
    /* Create one more stack */
    custom_stack *stack_second = create_custom_stack(20);
    for (i = 0; i < 25; i++) {
        push(stack_second, 100 - i);
    }
    pop(stack_second);
    printf("Second stack info: \n");
    print_debug_info(stack_second, STACK_INFO);
    
    /* And call SIGSEGV  */
    *(int*)(stack_second->internal + 2 * sizeof(int)) = 21421;
    pop(stack_second);
    
    /* Free memory:  */
    free_buffer(stack);
    free_buffer(stack_second);
    free(stack->internal);
    free(stack_second->internal);
}
