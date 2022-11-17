/* -------------------------------------------------------------------------
 * This program simulates a single-server FIFO service node using arrival
 * times and service times read from a text file.  The server is assumed
 * to be idle when the first job arrives.  All jobs are processed completely
 * so that the server is again idle at the end of the simulation.   The
 * output statistics are the average interarrival time, average service
 * time, average delay in the queue, the average wait in the service
 * node.
 * The program uses the Next_Event_Simulation approach with the implementation
 * of a Future Event List (FEL).
 * Scheduling the arrivals of new jobs and their
 * completions when they are served after (possibly) spending time in the input
 * queue sets the ground for simulating single server systems of this type with
 * different queueing and service policies.
 * The simulation stops when an END_Simulation event "emerges" from the FEL,
 * meaning that all the arrived jobs have been processed. To ensure this to occurr
 * correctly an END-of-Simulation event is scheduled during the initialization
 * of the simulator for an extremely large End-of-Simulation time.

 *
 * Name              : NESssq.c  (Next_Event Simulation of Single Server Queue)
 * Authors           : Gianfranco Balbo
 * Language          : ANSI C
 * Latest Revision   : 8-12-2017
 * -------------------------------------------------------------------------
 */
#include "NESssq.h"
#include "NESssq_List_Manager.h"
#include <malloc.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
double Start;        /* Beginnig of Observation Period */
double Stop;         /* End of Observation Period */
double ObservPeriod; /* Length of the Observation Period */
double clock;        /* Clock of the simulator - Simulation time */
double oldclock;
double lasttime;    /* time of last processed event*/
double End_time;    /* maximum simulation time */
double Target_time; /* maximum simulation time */
FILE *fp;
dll FEL;
dll Input_Queue;
dll AL;

int job_number;  /* (progressive) Job identification number */

double arrival_t; /* time of arrival read from trace */
double service_t; /* service time read from trace */

boolean halt; /* End of simulation flag */
int nsys;     /* Number of customers in system */
int nsysMax;  /* Maximum numbwr of customers in system */
int narr;     /* Number of Arrivals */
int ncom;     /* Number of Completions */
int ndelayed; /* Number of Delayed customers */

int event_counter; /* Number of the events processed by the simulkator*/
int node_number;   /* actual number of nodes used by the simulator */
int return_number; /* Number of nodes used by the simulator */

// ******************** EXERCISE 1 ***********************
double Area_n;       /* Integral of the function nsys(t)  = Area_w*/
double Area_d;       /* Integral of the function nqueue(t)  = Area_w*/
double Busy;         /* Busy time */
double Last_arrival; /* Timeof last arrival */

double E_of_sigma; /* Average inter-arrival time */
double E_of_S;     /* Average service time */
double E_of_D;     /* Average delay time */
double E_of_W;     /* Average wait time */

double U;      /* Utilization */
double X;      /* Throughput */
double Theta;  /* Input rate */
double Lambda; /* Arrival rate */
double Mu;     /* Arrival rate */
double Rho;    /* Traffic intensity */
double E_of_q; /* Average number of customers in queue */
double E_of_y; /* Average number of customers in service */
double E_of_n; /* Average number of customers at server */

int n_target;
int target = 100;
int main(int argc, char *argv[])
{
    int i;
    fp = fopen(FILENAME, "r");
    if (fp == NULL)
    {
        fprintf(stderr, "Cannot open input file %s\n", FILENAME);
        return (1);
    }
    /* Simulation Run */
    if (!feof(fp))
    {
        simulate();
    }
    return 0;
}

void simulate()
{
    /* Simulation core */
    initialize();
    while (!(halt))
        engine();
    report();
}

void engine(void)
{
    int event_type;
    double interval;
    nodePtr new_event;
    /* Get the first event notice from Future Event List */
    new_event = event_pop();

    /* update clock */
    // Exercise 1 - Begin
    oldclock = clock;
    // Exercise 1 - END
    clock = new_event->event.occur_time;
    // Exercise 1 - Begin
    interval = clock - oldclock;
    // Exercise 1 - END

    /* Collect basic statistics */
    // Exercise 1 - Begin

    if (nsys > 0)
    {
        Busy = Busy + interval;
        Area_n = Area_n + nsys * interval;
        Area_d = Area_d + (nsys - 1) * interval;
    }
    // Exercise 1 - End

    /* Identify and process current event */
    event_type = new_event->event.type;
    switch (event_type)
    {
    case ARRIVAL:
        arrival(new_event);
        break;
    case DEPARTURE:
        departure(new_event);
        break;
    case PROBE:
        probe(new_event);
        break;
    case END:
        end(new_event);
        break;
    }
    event_counter++;
    lasttime = clock;
    print_FEL();
}

void initialize()
{
    /* event notice used to store in the Future Event List the
       records corresponding to the events known to occur (in the future)
       at the moment of the initialization */
    nodePtr curr_notice;

    /* Control Settings  */
    halt = FALSE;
    /* Basic  counters  */
    job_number = 1;
    event_counter = 0;
    node_number = 0;

    /* Future Event List */
    FEL.Head = NULL;
    FEL.Tail = NULL;
    /* Input Queue of Server */
    Input_Queue.Head = NULL;
    Input_Queue.Tail = NULL;
    /* Available List */
    AL.Head = NULL;
    AL.Tail = NULL;

    /* Basic Statistic Measures  */
    nsys = 0;
    nsysMax = 0;
    narr = 0;
    ncom = 0;
    ndelayed = 0;
    clock = 0.0;

    // **************** EXERCISE 1 ***********************
    Start = 0.0;
    Busy = 0;
    Area_n = 0.0;
    Area_d = 0.0;

    /* Set Maximum Simulation length */
    End_time = ENDTIME;
    Target_time = target;

    /* Get first Arrival and Service Time pair*/
    arrival_t = GetArrival(fp);
    service_t = GetService(fp);

    if (!feof(fp))
    {

        /* Initialize Event notice of first arrival and Schedule first event */
        curr_notice = get_new_node();
        curr_notice->event.type = ARRIVAL;
        curr_notice->event.create_time = clock;
        curr_notice->event.occur_time = arrival_t;
        curr_notice->event.arrival_time = arrival_t;
        curr_notice->event.service_time = service_t;
        curr_notice->next = NULL;
        curr_notice->previous = NULL;
        sprintf(curr_notice->event.name, "J%d", (job_number++));
        schedule(curr_notice);

        /* Initialize Event notice of Target time and Schedule Probe event*/
        curr_notice = get_new_node();
        curr_notice->event.type = PROBE;
        curr_notice->event.create_time = clock;
        curr_notice->event.occur_time = Target_time;
        curr_notice->next = NULL;
        curr_notice->previous = NULL;
        sprintf(curr_notice->event.name, "PROBE");
        schedule(curr_notice);

        /* Initialize Event notice of End Simulation and Schedule last event*/
        curr_notice = get_new_node();
        curr_notice->event.type = END;
        curr_notice->event.create_time = clock;
        curr_notice->event.occur_time = End_time;
        curr_notice->next = NULL;
        curr_notice->previous = NULL;
        sprintf(curr_notice->event.name, "END");
        schedule(curr_notice);
    }
    printf("\nInitial Future Event List:");
    print_FEL();
    printf("\nInitial Server_Queue:");
    print_list(&Input_Queue);
}

void arrival(nodePtr node_event)
{
    /* manage arrival event */
    nodePtr next_job;

    /* Update statistics */
    nsys++;
    if (nsys > nsysMax)
        nsysMax = nsys;
    narr++;

    node_event->event.create_time = clock;
    node_event->event.arrival_time = clock;

    if (nsys == 1)
    {
        /* Process arrival at idle server */
        node_event->event.type = DEPARTURE;
        node_event->event.occur_time = clock + node_event->event.service_time;
        schedule(node_event);
    }
    else
    {
        /* Process arrival at busy server */
        node_event->event.type = NO_EVENT;
        node_event->event.occur_time = 0.0;
        enqueue(node_event, &Input_Queue);
        ndelayed++;
    }
    if (!feof(fp))
    {
        /* get new pair fromn trace */
        arrival_t = GetArrival(fp);
        service_t = GetService(fp);

        /* schedule new arrival */
        next_job = get_new_node();
        next_job->event.type = ARRIVAL;
        next_job->event.create_time = clock;
        next_job->event.occur_time = arrival_t;
        next_job->event.service_time = service_t;
        next_job->event.arrival_time = arrival_t;
        next_job->next = NULL;
        next_job->previous = NULL;
        sprintf(next_job->event.name, "J%d", (job_number++));
        schedule(next_job);
    }
    else
        // Exercise 1 - Begin
        Last_arrival = clock;
    // Exercise 1 - End
}

void departure(nodePtr node_event)
{
    /* manage departure event */
    nodePtr next_job;

    /* Update statistics */
    nsys--;
    ncom++;

    if (nsys > 0)
    {
        /* Process departure from a server with a queue*/
        next_job = dequeue(&Input_Queue);
        next_job->event.type = DEPARTURE;
        next_job->event.occur_time = clock + next_job->event.service_time;
        schedule(next_job);
    }
    return_node(node_event);
    return_number++;
}

void probe(nodePtr node_event)
{
    /* manage END event */
    n_target = nsys;
}

void end(nodePtr node_event)
{
    /* manage END event */
    halt = TRUE;
    /* Observation period ends with the moment of last deprture */
    printf("\nLastTime = %10.6f,   OldClock = %10.6f", lasttime, oldclock);
    Stop = lasttime;
    return_node(node_event);
    return_number++;
}

/* =========================== */
double GetArrival(FILE *fp) /* read an arrival time */
/* =========================== */
{
    double a;

    fscanf(fp, "%lf", &a);
    return (a);
}

/* =========================== */
double GetService(FILE *fp) /* read a service time */
/* =========================== */
{
    double s;

    fscanf(fp, "%lf\n", &s);
    return (s);
}

void print_results()
{

    printf("\n-------------------------------------------------------------------------");
    printf("\nSIMULATION RUN STATISTICS");
    printf("\n-------------------------------------------------------------------------");
    printf("\nNumber of processed events            = %d", event_counter);
    printf("\nMaximun number of customers in system = %d", nsysMax);
    printf("\nNumber of new generated nodes         = %d", node_number);
    printf("\nNumber of used nodes                  = %d", return_number);
    printf("\nNumber of arrivals    = %d; ......... Number of completions = %d", narr, ncom);
    printf("\nNumber of delayed customers           = %d", ndelayed);
    printf("\nStart Simulation Time = %10.6f; ... Last Simulation Time  = %10.6f", Start, Stop);
    printf("\nLength of Observation Period          = %10.6f", ObservPeriod);
    printf("\nLast Future Event List:");
    print_FEL();
    printf("\nLast Server_Queue:");
    print_list(&Input_Queue);
    printf("\n-------------------------------------------------------------------------");
    printf("\n\n************************");
    printf("\n*                      *");
    printf("\n*  SIMULATION RESULTS  *");
    printf("\n*                      *");
    printf("\n************************\n");

    // ************************ EXERCISE 1 **********************
    printf("\n EXERCISE 1");
    printf("\n   Average inter_arrival time ...................... = %14.10f", E_of_sigma);
    printf("\n   Average service time ............................ = %14.10f", E_of_S);
    printf("\n   Average delay time .............................. = %14.10f", E_of_D);
    printf("\n   Average Waiting Time in the Server .............. = %14.10f", E_of_W);
    printf("\n");
    printf("\n   Input rate ...................................... = %14.10f", Theta);
    printf("\n   Service rate .................................... = %14.10f", Mu);
    printf("\n   Traffic intensity ............................... = %14.10f", Rho);
    printf("\n   Throughput ...................................... = %14.10f", X);
    printf("\n   Server Utilization .............................. = %14.10f", U);
    printf("\n   Average number at server ........................ = %14.10f", E_of_n);
    printf("\n   Average number in queue ......................... = %14.10f", E_of_q);
    printf("\n   Average number in service ....................... = %14.10f", E_of_y);
    printf("\n-------------------------------------------------------------------------\n");
    printf("\n EXERCISE 2");
    printf("\n   Number of customers in the system at time %f .... = %d", Target_time, n_target);
}

double GetInterArrival(FILE* fp)
{
    return 0.0;
}

void report()
{

    ObservPeriod = Stop - Start;

    // ************************ EXERCISE 1 **********************

    E_of_sigma = Last_arrival / narr; /* Average inter-arrival time */
    E_of_S = Busy / ncom;             /* Average service time */
    E_of_D = Area_d / ncom;           /* Average delay time */
    E_of_W = Area_n / ncom;

    U = Busy / ObservPeriod;        /* Utilization */
    X = ncom / ObservPeriod;        /* Throughput */
    Theta = narr / Last_arrival;    /* Input rate */
    Lambda = narr / ObservPeriod;   /* Arriva rate */
    Mu = ncom / Busy;               /* Service rate */
    Rho = Busy / Last_arrival;      /* Traffic intensity */
    E_of_q = Area_d / ObservPeriod; /* Mean number of customers in queue */
    E_of_y = Busy / ObservPeriod;   /* Mean number of customers in service */
    E_of_n = Area_n / ObservPeriod; /* Mean number of customers at server */

    print_results();

    destroy_list(FEL.Head);
    destroy_list(Input_Queue.Head);
    destroy_list(AL.Head);
}
