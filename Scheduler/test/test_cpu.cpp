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
    cpu->Process(new Event("TEST", ARRIVAL, 0, 0, 0, 0, CPU));
    auto nextEvt = &mockScheduler->EvtList.Dequeue();
    ASSERT_EQ(nextEvt->Station, CPU);
    ASSERT_EQ(nextEvt->GeneratedNodes, 1);
    ASSERT_GE(nextEvt->OccurTime, 0);
    ASSERT_EQ(nextEvt->Type, DEPARTURE);
}

TEST_F(CpuTest, test_departure)
{
    cpu->Process(new Event("TEST", DEPARTURE, 0, 10, 0, 10, CPU));
    auto nextEvt = &mockScheduler->EvtList.Dequeue();
    assert(nextEvt->Station == IO_2 || nextEvt->Station == IO_1 || nextEvt->Station == SWAP_OUT);
}

TEST_F(CpuTest, test_renqueue)
{
    cpu->Process(new Event("TEST",DEPARTURE,0,10,5,0,CPU));
    auto nextEvt = &mockScheduler->EvtList.Dequeue();
    ASSERT_EQ(nextEvt->ServiceTime,5);
    ASSERT_EQ(nextEvt->Station,CPU);
}