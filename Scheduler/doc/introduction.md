# Descrizione del sistema

## Code
|Coda| Tipo di coda| Tipo di processo|
|---|---|---|
|Job List| FCFS| Processi che sono entrati nel sistema
|Ready queue| FCFS| processi che aspettano o sono pronti per essere eseguiti
|Device queue| FCFS| lista di processi che aspettano di poter accedere a un determinato dispositivo IO
|workstation pool| infinite server| praticamente stazione di delay da cui i processi vengono generati
|Reserve station| FCFS| Processi che sono in attesa di essere ammessi (allocati) nel sistema

## Parametri
|Parametro| Tipo di parametro| Descrizione
|---|---|---|
| Degree of multiprogramming| Int| delimita il numero di processi che possono essere ammessi nella memoria principale
|Time slice length| double| definisce il limite superiore del quanto di tempo che può essere dedicato a un singolo processo che gira nella CPU
|N |  int| numero di workstation connesse al server, sempre occupate da user

- Average work-station think time Z = 5000 msec,
- Average swap-in time S1 = 210 msec,
- Average CPU time S2 = 27 msec
- Length of the time slice (CPU quantum) δ = 2.7 msec
- Average I/O1 time s4 = 40 msec
- Average I/O2 time s4 = 180 msec
- CPU completion choice q3,4 = 0.65, q3,5 = 0.25, q3,6 = 0.1
- Swap-out choice q6,0 = 0.4, q6,1 = 0.6.
- Multi-Programming Degree M P D = 10

tutte le stazioni usano una distribuzione esponenziale, eccetto la CPU che ha un time slice deterministico e la lunghezza totale del burst che ha un distribuzione iper esponenziale a doppio stadio

$f_X(x) = \alpha*\frac{1}{\mu_1}*exp(\frac{-x}{\mu_1}) + \beta*\frac{1}{\mu_2}*exp(\frac{-x}{\mu_2})$ 
con parametri:
- $\alpha= 095$
- $\beta = 0.05$
- $\mu_1 = 10msec$
- $\mu_2 = 350msec$

## Fasi di lavoro

- Workstation submit a process:
  - Out: a process is submitted at the reserve station
- Reserve station:
  - In: a process submitted from the workstation
  - Process: the process waits at the station to be admitted in the system, admission made using the MPD
  - Out: a process admitted with an allocated memory block, request to Swap in node to load the process image 
- Swap in
  - In: a process submitted by the reservation station
  - Process: process becomes ready to be executed 
  - Out: the process is submitted to the CPU queue and ready to be executed 
- CPU Queue
  - In: a process submitted by the swap in 
  - Process: the process is processed by the CPU for a certain time defined on the basis of possible interruption, either by the time slice (expired time) or because the process is terminated or because it needs to access an IO device
  - Out: a process that need to be reprocessed or have requested access to an IO device or is swapped out to the reserve station again (in the last 2 cases the memory is freed and another process can be admitted)
- IO Queue
  - In: a process that requested the access to the IO device
  - Process: process access to IO for an amount of time
  - Out: the process is submitted to the CPU queue again and becomes ready 
  

# Eventi

- Arrivals:
  - Reserve Station
  - Swap In 
  - CPU Queue 
  - IO 1/2
- Departure:
  - Reserve Station
  - Swap In 
  - CPU Queue 
  - IO 1/2
- Time Slice expired

