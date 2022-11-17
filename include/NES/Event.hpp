#include <iostream>


enum class EventType : int
{
	NO_EVENT = -1,
	ARRIVAL,
	DEPARTURE,
	END,
	PROBE
};


struct Event
{
	double CreateTime = 0.0;
	double OccurTime = 0.0;
	double ServiceTime = 0.0;
	double ArrivalTime = 0.0;
	std::string Name{};
	EventType Type = EventType::NO_EVENT;
	Event(){}
	Event(std::string name,EventType type,double createTime, double occurTime, double serviceTime, double arrivalTime): Name{name}, Type{type}, CreateTime{CreateTime}, OccurTime{occurTime}, ServiceTime{serviceTime}, ArrivalTime{arrivalTime}{}
	~Event()
	{
		Name.clear();
	}

	Event(const Event& e): CreateTime{e.CreateTime},ArrivalTime{e.ArrivalTime},ServiceTime{e.ServiceTime},Name{e.Name},OccurTime{e.OccurTime}
	{
		
	}

	Event(Event&& e): Event{e}
	{
		delete &e;
	}


};