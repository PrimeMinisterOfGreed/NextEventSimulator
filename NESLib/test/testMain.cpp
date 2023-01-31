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
    handler += [&]()
    { mutex.unlock(); };
    auto ret = std::async([&]()
                          {
                              sleep(1);
                              handler.Invoke();
                          });
    mutex.lock();
}

TEST(TestEventHandler, test_handler_deletion)
{
    std::mutex mutex;
    EventHandler handler;
    bool target = false;
    std::function<void()> fnc = [&](){target = true;};
    handler += fnc;
    handler -= fnc;
    handler.Invoke();
    ASSERT_EQ(target, false);


}