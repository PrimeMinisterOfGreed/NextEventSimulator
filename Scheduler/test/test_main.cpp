#include <gtest/gtest.h>
#include "Generators.hpp"

std::shared_ptr<IGenerator> streamGenerator;
int main(int argc, char ** argv)
{
    testing::InitGoogleTest(&argc, argv);
    streamGenerator = std::shared_ptr<StreamGenerator>(new StreamGenerator());
    return RUN_ALL_TESTS();
}

TEST(test, test)
{
    auto val = streamGenerator.get();
}