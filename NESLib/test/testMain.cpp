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
    handler += new FunctionHandler<>([&mutex]()
                                      { mutex.unlock(); });
    auto ret = std::async([&]()
                          {
                              sleep(1);
                              handler.Invoke();
                          });
    mutex.lock();
}

void fnc(bool *a)
{ *a = true; }

TEST(TestEventHandler, test_handler_deletion)
{
    bool a = false;
    EventHandler<bool *> handler;
    FunctionHandler<bool *> fnc([](bool *a)
                                { *a = false; });
    handler += &fnc;
    handler -= &fnc;
    handler.Invoke(&a);
    ASSERT_EQ(a, false);
}

