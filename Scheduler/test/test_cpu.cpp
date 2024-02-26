//
// Created by drfaust on 26/01/23.
//
#include "CPU.hpp"
#include "Enums.hpp"
#include "Event.hpp"
#include "IOStation.hpp"
#include "LogEngine.hpp"
#include "MockStation.hpp"
#include "Scheduler.hpp"
#include "Station.hpp"
#include "SystemParameters.hpp"
#include "rngs.hpp"
#include <fmt/core.h>
#include <gtest/gtest.h>
#include <ios>

TEST(TestCpu, test_arrival)
{
    LogEngine::CreateInstance("test.txt");
    Scheduler sched{"scheduler"};
    auto cpu = new Cpu(&sched);
    sched.AddStation(cpu);
    auto evt = sched.Create(10, 0);
    evt.Station = 3;
    sched.Schedule(evt);
    while (sched.ProcessNext().ServiceTime > 0)
        ;
}

TEST(TestCpu, test_flooding)
{
    Scheduler sched{"scheduler"};
    auto &params = SystemParameters::Parameters();
    auto cpu = new Cpu(&sched);
    auto io1 = new MockStation(Stations::IO_1);
    auto io2 = new MockStation(Stations::IO_2);
    auto swap_out = new MockStation(Stations::SWAP_OUT);
    params.burstmode = SystemParameters::FIXED;
    params.cpuQuantum = 2.7;
    params.cpuChoice = std::vector<double>{0.065, 0.025, 0.01, 0.9};
    sched.AddStation(io1);
    sched.AddStation(io2);
    sched.AddStation(swap_out);
    sched.AddStation(cpu);
    for (int i = 0; i < 10; i++)
    {
        auto evt = sched.Create(1, 27);
        evt.Station = CPU;
        sched.Schedule(evt);
    }
    while (sched.HasEvents())
    {
        sched.ProcessNext();
        fmt::println("Cpu U:{}, W:{}, N:{}", cpu->utilization(), cpu->avg_waiting(),cpu->sysClients());
    }
}

TEST(TestIO, test_arrival)
{
    RandomStream::Global().PlantSeeds(123456789);
    Scheduler sched{"scheduler"};
    auto cpu = new MockStation(CPU);
    auto io1 = new MockStation(Stations::IO_1);
    auto io2 = new IOStation(&sched, IO_2);
    auto swap_out = new MockStation(Stations::SWAP_OUT);
    sched.AddStation(io1);
    sched.AddStation(io2);
    sched.AddStation(swap_out);
    sched.AddStation(cpu);
    SystemParameters::Parameters().averageIO2 = 40000;
    for (int i = 0; i < 20; i++)
    {
        auto evt = sched.Create(i, i);
        evt.Station = IO_2;
        sched.Schedule(evt);
    }
    for (int i = 0; i < 20; i++)
    {
        sched.ProcessNext();
        ASSERT_EQ(io2->sysClients(), i + 1);
    }
    for (int i = 0; i < 20;)
    {
        auto evt = sched.ProcessNext();
        if (evt.Type == DEPARTURE)
        {
            ASSERT_EQ(io2->sysClients(), 20 - i - 1);
            i++;
        }
    }
}