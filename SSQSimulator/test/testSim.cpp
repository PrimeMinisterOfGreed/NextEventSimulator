#include <gtest/gtest.h>
#include <boost/program_options.hpp>

boost::program_options::variables_map optionsMap = *new boost::program_options::variables_map();
int endTime = 20000;
int targetTime = 100;

int main(int argc, char* argv[])
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

TEST(test_simulator, simulator)
{
}

