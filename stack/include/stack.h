/** STACK_INFO is used to print basic stack info and STACK_ERR for full stack
*   info
*/
#define STACK_INFO 0
#define STACK_ERR 1

/**
* @brief Stack with internal data. This data will be implemented in compiled
*        file.
*/
typedef struct {
     void *internal; /** All stack data  */
    
} custom_stack;


/**
* @brief Create stack structure pointer.
* @param capacity - default capacity of the stack.
* @return created pointer to the stack,
*         NULL if error
*/
custom_stack *  create_custom_stack(int capacity);

/**
* @brief Push element to stack.
* @param stack - target stack structure pointer
* @param elem - element that will be pushed in
*/
void push(custom_stack *stack, double elem);

/**
* @brief Pop element from stack.
* @param stack - target stack structure pointer
*/
void pop(custom_stack *stack);

/**
* @brief Get top element from stack
* @param stack - target stack pointer
* @return top element
*/
double top(custom_stack *stack);

/**
* @brief Print out info about stack in 2 modes.
* @param stack - target stack pointer
* @param MODE - output mode
* @note if MODE == STACK_ERR then work with errno flags
*/
void print_debug_info(custom_stack *, int MODE);

/**
* @brief Dump all written stacks. Called if SIGSEGV was catched 
*/
void dump_all_stacks();

/**
* @brief Clear stack buffer.
* @param stack - target stack pointer
*/
void free_buffer(custom_stack *stack);
