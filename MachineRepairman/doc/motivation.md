## Events

The system is altered from those kind of events:
- Arrival
- Departure
- Maintenance 

Arrival and Departure identify activity of a client: it arrive at the queue and then leave after being served. Maintenance instead is threated as a special kind of client: when it arrive the workshift of the repairman is over, meanwhile the end of its service (the departure) is the start of the workshift of the repairman. 

## State 

the system at t=0 correspond to the start of the first workshift of the repairman, the total workshift is 480 min so at this time a Maintenance event will be enqueued with occur time 480. 


