#include "structs.h"

node * generate_process(unsigned long num_process);
void *execute_process(void *new_process);
void *process(void *current);
node *get_first();
node *get_last();
void add_process (node *newnode);