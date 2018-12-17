#include "../include/stack.h"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

/* Terminal colors:  */
#define RED "\x1B[31m"
#define RESET "\x1B[0m"

void signal_handler(int signum)
{
   printf(RED "Caught SIGSEGV\nDump:\n" RESET);
   dump_all_stacks(); 
   exit(signum);
}
