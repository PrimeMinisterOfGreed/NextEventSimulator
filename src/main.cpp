#include "ssq1.hpp"
#include "NESssqv2.hpp"
#include "DataProvider.hpp"
int main(int argc, char* argv[])
{
	 NESssq simulator(new FileDataProvider(* new std::string("ssq2.dat"),true));
	simulator.Execute();
	simulator.Report();
}

