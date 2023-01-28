#include <gtest/gtest.h>
#include "EventHandler.hpp"
#include <mutex>
#include <thread>
#include <future>

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}


TEST(TestEventHandler, test_handling)
{
    std::mutex mutex;
    EventHandler handler;
    handler += [&](){mutex.unlock();};
    std::async([&](){
        sleep(1);
        mutex.unlock();
    });
    mutex.lock();
}