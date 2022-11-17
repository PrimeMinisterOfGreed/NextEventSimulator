#pragma once
#include "NESssq.h"
#include <stdio.h>

nodePtr get_new_node();
void return_node(nodePtr item);
nodePtr pop_from_AL();
void push_on_AL(nodePtr item);
void schedule(nodePtr new_node_event);
nodePtr event_pop();
void enqueue(nodePtr new_node, dll *curr_queue);
nodePtr dequeue(dll *curr_queue);
void destroy_list(nodePtr p);
void print_FEL();
void print_list(dll *dll);

void print_node(nodePtr node);