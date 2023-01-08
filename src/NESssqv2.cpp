#include "NESssqv2.hpp"



void NESssq::Arrival(Event& e)
{
	_statistic.nsys++;
	systemClients(_statistic.nsys);
	_statistic.narr++;

	e.CreateTime = _clock.clock;
	e.ArrivalTime = _clock.clock;
	if (_statistic.nsys == 1)
	{
		e.Type = EventType::DEPARTURE;
		e.OccurTime = _clock.clock + e.ServiceTime;
		Schedule(e);
	}
	else
	{
		e.Type = EventType::NO_EVENT;
		e.OccurTime = 0.0;
		_inputQueue.Enqueue(new Event(e));
		_statistic.ndelayed++;
	}
	if (!_provider->end())
	{
		auto arrival = _provider->GetArrival();
		auto service = _provider->GetService();
		Event& newEvent = *new Event(std::string("J") + std::to_string(_statistic.narr), EventType::ARRIVAL, _clock.clock, arrival, service, arrival);
		Schedule(newEvent);
	}
	else
		_statistic.Last_arrival = _clock.clock;
}

void NESssq::Schedule(Event& e)
{
	std::cout << "Scheduled: " << e.Name << ",T:" << e.OccurTime << std::endl;
	_futureEvents.Insert(&e, [](const Event&a,const Event& b) { return a.OccurTime > b.OccurTime; });
}

void NESssq::Departure(Event& e)
{
	_statistic.nsys--;
	_statistic.ncom++;
	systemClients(_statistic.nsys);
	if (_statistic.nsys > 0)
	{
		Event& newE = _inputQueue.Dequeue();
		newE.Type = EventType::DEPARTURE;
		newE.OccurTime = _clock.clock + newE.ServiceTime;
		Schedule(newE);
	}
	delete& e;
	_statistic.return_number++;
}

void NESssq::End(Event& e)
{
	_stop = true;
	std::cout << "Last Time= " << std::to_string(_clock.lasttime) << ", Old Clock: " << std::to_string(_clock.oldclock) << std::endl;
	_clock.Stop = _clock.lasttime;
	e.~Event();
	_statistic.return_number++;
}

void NESssq::Probe(Event& e)
{
	_statistic.clientTarget = _statistic.nsys;
}

void NESssq::Initialize()
{
	_clock.End_time = endTime;
	_statistic.Target_time = targetTime;
	_provider->Next();
	_clock.Arrival = _provider->GetArrival();
	_clock.Service = _provider->GetService();
	if (!_provider->end())
	{
		Schedule(*new Event(std::string("J") + std::to_string(jobIndex), EventType::ARRIVAL, _clock.clock, _clock.Arrival, _clock.Arrival, _clock.Service));
		Schedule(*new Event(std::string("PROBE"), EventType::PROBE, _clock.clock, _statistic.Target_time, 0, 0));
		Schedule(*new Event(std::string("END"), EventType::END, _clock.clock, _clock.End_time, 0, 0));
	}
	std::cout << std::string("Initial Future Event List: ") << _futureEvents.ToString() << std::endl;
	std::cout << std::string("Initial Server queue: " )<< _inputQueue.ToString() << std::endl;
}

void NESssq::Execute()
{
	Initialize();
	while (!_stop)
	{
		_provider->Next();
		Engine();
	}
	Report();
}

void NESssq::Engine()
{
	auto& newEvent = _futureEvents.Pull();
	_clock.oldclock = _clock.clock;
	_clock.clock = newEvent.OccurTime;
	double interval = _clock.clock - _clock.oldclock;
	if (_statistic.nsys > 0)
	{
		_clock.Busy = _clock.Busy + interval;
		_statistic.Area_n += _statistic.nsys * interval;
		_statistic.Area_d += (_statistic.nsys - 1)* interval;
	}
	switch (newEvent.Type)
	{
		case EventType::ARRIVAL:
		Arrival(newEvent);
		break;
		
		case EventType::DEPARTURE:
		Departure(newEvent);
		break;

		case EventType::PROBE:
		Probe(newEvent);
		break;

		case EventType::END:
		End(newEvent);
		break;
		default:
		break;
	}
	_statistic.event_counter++;
	_clock.lasttime = _clock.clock;
}

void NESssq::Report()
{
	ISSQSimulator::Report();

	_clock.ObservPeriod = _clock.Stop - _clock.Start;

	// ************************ EXERCISE 1 **********************

	_statistic.E_of_sigma = _statistic.Last_arrival / _statistic.narr; /* Average inter-arrival time */
	_statistic.E_of_S = _clock.Busy / _statistic.ncom;             /* Average service time */
	_statistic.E_of_D = _statistic.Area_d / _statistic.ncom;           /* Average delay time */
	_statistic.E_of_W = _statistic.Area_n / _statistic.ncom;

	_statistic.U = _clock.Busy / _clock.ObservPeriod;        /* Utilization */
	_statistic.X = _statistic.ncom / _clock.ObservPeriod;        /* Throughput */
	_statistic.Theta = _statistic.narr / _statistic.Last_arrival;    /* Input rate */
	_statistic.Lambda = _statistic.narr / _clock.ObservPeriod;   /* Arriva rate */
	_statistic.Mu = _statistic.ncom / _clock.Busy;               /* Service rate */
	_statistic.Rho = _clock.Busy / _statistic.Last_arrival;      /* Traffic intensity */
	_statistic.E_of_q = _statistic.Area_d / _clock.ObservPeriod; /* Mean number of customers in queue */
	_statistic.E_of_y = _clock.Busy / _clock.ObservPeriod;   /* Mean number of customers in service */
	_statistic.E_of_n = _statistic.Area_n / _clock.ObservPeriod; /* Mean number of customers at server */

	std::cout << "Statistics---" << std::endl << _statistic.ToString()
		<< std::endl << "Clock---" << _clock.ToString() << std::endl
		<< "Future Event List:" << _futureEvents.ToString() << std::endl
		<< "Input Queue: " << _inputQueue.ToString() << std::endl;
}

const std::string & EventList::ToString()
{
	std::stringstream buffer{};
	auto ptr = begin();
	while (ptr != end() &&  &ptr() != nullptr)
	{
		buffer << "|Job:" << (*ptr).Name << ",T:" << std::to_string((*ptr).OccurTime) << "|-->";
		ptr++;
	}
	return *new std::string(buffer.str());
}

std::string& Statistics::ToString()
{
	printf("\nNumber of processed events            = %d", event_counter);
	printf("\nMaximun number of customers in system = %d", nsysMax);
	printf("\nNumber of new generated nodes         = %d", node_number);
	printf("\nNumber of used nodes                  = %d", return_number);
	printf("\nNumber of arrivals    = %d; ......... Number of completions = %d", narr, ncom);
	printf("\nNumber of delayed customers           = %d", ndelayed);


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

	printf("\n   Number of customers in the system at time %f .... = %d", Target_time, n_target);
	return *new std::string();
}

std::string& Clock::ToString()
{
	printf("\nStart Simulation Time = %10.6f; ... Last Simulation Time  = %10.6f", Start, Stop);
	printf("\nLength of Observation Period          = %10.6f", ObservPeriod);
	
	return *new std::string();
}
