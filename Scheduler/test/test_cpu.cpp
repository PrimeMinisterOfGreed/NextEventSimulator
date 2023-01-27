//
// Created by drfaust on 26/01/23.
//
#include <gtest/gtest.h>
#include "CPU.hpp"
#include "Enums.hpp"

class MockScheduler : public IScheduler
{
public:
    EventList &EvtList = *new EventList();

    void Schedule(Event *event) override
    {
        EvtList.Insert(event, [](const Event &a, const Event &b)
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
public:
    Cpu *cpu;
    MockScheduler *mockScheduler;
protected:
    virtual void TearDown()
    {
        cpu->Reset();
        delete mockScheduler;
    }

    virtual void SetUp()
    {
        ConsoleLogEngine::CreateInstance(3, "testLog.txt");
        mockScheduler = new MockScheduler();
        cpu = new Cpu(ConsoleLogEngine::Instance(), mockScheduler, 2.7);
    }
};


TEST_F(CpuTest, test_instancing)
{

}

TEST_F(CpuTest, test_enqueue)
{
    cpu->Process(new Event("TEST",ARRIVAL,0,0,0,0,CPU));

}