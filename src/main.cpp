#include "SolverApp.h"
#include "Configuration.h"

#include <iostream>
#include <stdexcept>

int main(int argc, char** argv)
{
	try 
	{
		Configuration config(argc, argv);
		SolverApp app(config);
		app.run();
	}
	catch (std::runtime_error& ex)
	{
		std::cout << ex.what() << std::endl;
		return -1;
	}
	return 0;
}
