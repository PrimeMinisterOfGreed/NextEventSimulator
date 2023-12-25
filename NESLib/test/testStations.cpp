#include "DataCollector.hpp"
#include "Event.hpp"
#include "LogEngine.hpp"
#include "Measure.hpp"
#include "Station.hpp"
#include "gtest/gtest.h"

TEST(TestStation, test_onarrival)
{
    LogEngine::CreateInstance("test");
    int a = 0;
    BaseStation s{"test"};
    s.OnArrival([&a](auto s, Event &e) { a++; });
    auto evt = Event("test", ARRIVAL, 0, 0, 0, 0, 0);
    s.Process(evt);
    ASSERT_EQ(1, a);
}

TEST(TestStation, test_fcfs_arrival)
{
}