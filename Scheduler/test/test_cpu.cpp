//
// Created by drfaust on 26/01/23.
//
#include <gtest/gtest.h>
#include "CPU.hpp"

class MockScheduler : public IScheduler
{
public:
	EventList& EvtList = *new EventList();

	void Schedule(Event* event) override
	{
		EvtList.Insert(event, [](const Event& a, const Event& b)
			{
				return a.OccurTime > b.OccurTime;
			});
	}

	virtual ~MockScheduler()
	{
		EvtList.Clear();
	}


};

class CpuTest : public ::testing::Test
{
private:
	Cpu* _cpu;
	MockScheduler* _mockScheduler;
protected:
	virtual void TearDown()
	{
		_cpu->Reset();
		delete _mockScheduler;
	}

	virtual void SetUp()
	{
		ConsoleLogEngine::CreateInstance(3, "testLog.txt");
		_cpu = new Cpu(ConsoleLogEngine::Instance(), _mockScheduler, 2.7);
	}
};


TEST_F(CpuTest, test_instancing)
{

}