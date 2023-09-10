//
// Created by drfaust on 26/01/23.
//
#include "CPU.hpp"
#include "Enums.hpp"
#include <gtest/gtest.h>

class MockScheduler : public IScheduler
{
  public:
    EventList EvtList{};

    void Schedule(Event event) override
    {
        EvtList.Insert(event, [](const Event &a, const Event &b) { return a.OccurTime > b.OccurTime; });
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
        LogEngine::CreateInstance(3, "testLog.txt");
        mockScheduler = new MockScheduler();
        cpu = new Cpu(mockScheduler);
    }
};

TEST_F(CpuTest, test_instancing)
{
}

TEST_F(CpuTest, test_enqueue)
{
    auto event = Event("TEST", ARRIVAL, 0, 0, 0, 0, CPU);
    cpu->Process(event);
    auto nextEvt = mockScheduler->EvtList.Dequeue();
    ASSERT_EQ(nextEvt.Station, CPU);
    ASSERT_EQ(nextEvt.GeneratedNodes, 1);
    ASSERT_GE(nextEvt.OccurTime, 0);
    ASSERT_EQ(nextEvt.Type, DEPARTURE);
}

TEST_F(CpuTest, test_departure)
{
    auto event = Event("TEST", DEPARTURE, 0, 10, 0, 10, CPU);
    cpu->Process(event);
    auto nextEvt = mockScheduler->EvtList.Dequeue();
    assert(nextEvt.Station == IO_2 || nextEvt.Station == IO_1 || nextEvt.Station == SWAP_OUT);
}

TEST_F(CpuTest, test_renqueue)
{
    auto event = Event("TEST", DEPARTURE, 0, 10, 5, 0, CPU);
    cpu->Process(event);
    auto nextEvt = mockScheduler->EvtList.Dequeue();
    ASSERT_EQ(nextEvt.ServiceTime, 5);
    ASSERT_EQ(nextEvt.Station, CPU);
}