#include <gtest/gtest.h>
#include "Generators.hpp"

IGenerator * streamGenerator;
int main(int argc, char ** argv)
{
    testing::InitGoogleTest(&argc, argv);
    streamGenerator = new StreamGenerator();
    return RUN_ALL_TESTS();
}

