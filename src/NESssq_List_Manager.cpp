
#include "NESssq_List_Manager.h"
#include "NESssq.h"
#include <malloc.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

extern dll FEL;
extern dll AL;
extern dll Input_Queue;

void enqueue(nodePtr new_node, dll *curr_queue)
{
    /* function to add an element at the tail of a generic queue (curr_queue) */
    if (curr_queue->Tail == NULL)
        /* curr_queue is empty*/
        curr_queue->Head = new_node;
    else
        /* add at the end of a non-empty curr_queue */
        curr_queue->Tail->next = new_node;
    /* adjust pointers */
    new_node->previous = curr_queue->Tail;
    new_node->next = NULL;
    curr_queue->Tail = new_node;
}

nodePtr dequeue(dll *curr_queue)
{
    /* function to remove the element at the head of a generic queue (curr_queue) */
    nodePtr item;
    if (curr_queue->Head == NULL)
        /* curr_queue is empty */
        return NULL;
    /* point to the element being removed from curr_queue */
    item = curr_queue->Head;
    if (curr_queue->Head->next == NULL)
    {
        /* curr_queue contains only one element that is being removed
        (leaving curr_queue empty) */
        curr_queue->Head = NULL;
        curr_queue->Tail = NULL;
    }
    else
    {
        /* adjust pointers to the new head of curr_queue */
        curr_queue->Head = curr_queue->Head->next;
        curr_queue->Head->previous = NULL;
    }
    /* clear the returned node*/
    item->previous = NULL;
    item->next = NULL;
    return item;
}

nodePtr get_new_node()
{
    nodePtr memory = (nodePtr)malloc(sizeof(struct node));
    memory->event = *(event_notice*)malloc(sizeof(event_notice));
    return memory;
}

void return_node(nodePtr item)
{
    free(item);
}

nodePtr pop_from_AL()
{
    nodePtr current = AL.Head;
    AL.Head = AL.Head->next;
    return current;
}

void push_on_AL(nodePtr item)
{
    nodePtr head = AL.Head;
    AL.Head = item;
    AL.Head->next = head;
}

void destroy_list(nodePtr p)
{
    if (p == NULL)
        return;
    while (p->previous != NULL)
        p = p->previous;
    while (p->next != NULL)
    {
        nodePtr toDestroy = p;
        p = p->next;
        return_node(toDestroy);
    }
}



void schedule(nodePtr new_node_event)
{
    printf("Scheduling event %s with occurrence at %f and service time %f \n", new_node_event->event.name, new_node_event->event.occur_time,new_node_event->event.service_time);
    nodePtr itr = FEL.Head;
    if (FEL.Head == NULL && FEL.Tail == NULL)
    {
        FEL.Head = new_node_event;
        FEL.Tail = new_node_event;
        FEL.Head->next = FEL.Tail;
    }
    else if (FEL.Head->event.occur_time > new_node_event->event.occur_time)
    {
        FEL.Head = new_node_event;
        new_node_event->next = itr;
        itr->previous = new_node_event;
    }
    else
    {
        while (itr != FEL.Tail && itr->event.occur_time < new_node_event->event.occur_time)
            itr = itr->next;
        if (itr == FEL.Tail)
        {
            new_node_event->previous = FEL.Tail;
            FEL.Tail->next = new_node_event;
            FEL.Tail = new_node_event;
        }
        else
        {
            nodePtr prev = itr->previous;
            prev->next = new_node_event;
            new_node_event->next = itr;
        }
    }
}

nodePtr event_pop()
{
    nodePtr result = FEL.Head;
    if (FEL.Head != NULL && FEL.Head->next != NULL)
    {
        FEL.Head = FEL.Head->next;
        FEL.Head->previous = NULL;
        result->next = NULL;
        result->previous = NULL;
    }
    else
    {
        FEL.Head = NULL;
        FEL.Tail = NULL;
    }
    return result;
}

void print_node(nodePtr node)
{
    printf("|%s,%f|->", node->event.name, node->event.occur_time);
}

void print_FEL()
{
    print_list(&FEL);
}

void print_list(dll *dll)
{
    if (dll->Head != NULL)
    {
        nodePtr start = dll->Head;
        while (start != dll->Tail)
        {
            print_node(start);
            start = start->next;
        }
        print_node(start);
        printf("\n");
    }
    else
    {
        printf("List Empty \n");
    }
}