#include "ssq1.hpp"

using namespace boost::accumulators;


void SSQSimulator::Execute()
{
	float arrival = 0;
	float oldArrival = 0 ;
	float departure = 0;
	float delay = 0;
	float wait = 0;
	float service = 0;
	while (!_provider->end())
	{
		jobIndex++;
		arrival = _provider->GetArrival();
		delay = arrival < departure ? departure - arrival : 0.0;
		delayedJobs += delay > 0 ? 1 : 0;
		service = _provider->GetService();
		if (_provider->end()) break;
		wait = delay + service;
		departure = arrival + wait;
		delays(delay);
		waits(wait);
		services(service);
		interarrivals(arrival - oldArrival);
		oldArrival = arrival;
		lastArrival = arrival;
	}
}

void SSQSimulator2::Execute()
{
	float interArrival = 0;
	float oldArrival = 0;
	float clock = 0;
	float departure = 0;
	float delay = 0;
	float wait = 0;
	float service = 0;
	while (!_provider->end())
	{
		jobIndex++;
		interArrival = _provider->GetArrival();
		clock += interArrival;
		delay = clock < departure ? departure - clock : 0.0;
		delayedJobs += delay > 0 ? 1 : 0;
		service = _provider->GetService();
		if (_provider->end()) break;
		wait = delay + service;
		departure = clock + wait;
		delays(delay);
		waits(wait);
		services(service);
		interarrivals(clock - oldArrival);
		oldArrival = clock;
		lastArrival = clock;
	}
}

void ISSQSimulator::Report()
{



}


