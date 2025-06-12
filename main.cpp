
#include "incl/Server.hpp"

volatile sig_atomic_t stopSignal = 0;

void signalHandler(int signum)
{
	std::cout << "\nReceived signal " << signum << ". Shutting down server.\n";
	stopSignal =  1;
}

int main(int ac, char **av)
{
	std::signal(SIGINT, signalHandler);
	
	if (ac != 2)
	{
		std::cout << "Please provide a config file [Usage: ./webserv *.conf]\n";
		return (1);
	}

	std::string configPath = checkFilePath(av[1]);
	if (configPath.empty())
	{
		std::cerr << "Invalid path for config file!\n";
		return (1);
	}

	std::vector<std::string>	configList;
	createConfigList(configPath, configList);

	if (configList.size() < 1)
	{
		std::cerr << "Loading server configuration failed!\n";
		return (1);
	}

	try
	{
		Server newServer(configList[0]);
		newServer.serverLoop();
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}

	return (0);
}
