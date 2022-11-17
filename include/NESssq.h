#pragma once
#include <stdio.h>

typedef enum boolean
{
    FALSE,
    TRUE
} boolean;

#define NO_EVENT -1
#define ARRIVAL 0
#define DEPARTURE 1
#define END 2
#define PROBE 3
#define FILENAME "ssq2.dat" /* input data file */

#define EVENT_NUMBER 50

#define TABLESIZE 20
#define ENDTIME 100000

/* Declarations and pointers dependent on the functions implemented to handle the basic data structures */
/*======================================================================================================*/

/* Definition of the type used to specify the pointer to a node of a queue*/
typedef struct node *nodePtr;
/* Definition of the type used to specify the header of a queue*/
typedef struct DLL
{
    nodePtr Head;
    nodePtr Tail;
} dll;
/* the Future Event List, the Input Queue and the Available List are declared as Doubly Linked Lists
   with a header having fields pointing to the first and to the last elements of the list */

/*======================================================================================================*/

typedef struct
{
    int type;
    char name[256];
    double create_time;
    double occur_time;
    double arrival_time;
    double service_time;
} event_notice;

struct node
{
    event_notice event;
    nodePtr previous; // Pointer to the previous node in the doubly linked list
    nodePtr next;     // Pointer to the next node in the doubly linked list
};

void simulate(void);
void initialize(void);
void engine(void);
nodePtr event_pop();
void arrival(nodePtr);
void departure(nodePtr);
void end(nodePtr);
void report(void);
void print_results(void);
double GetInterArrival(FILE *fp);
double GetService(FILE *fp);
void probe(nodePtr node_event);
void report1(void);

double GetArrival(FILE *fp);
