
#include "incl/Server.hpp" 
#include "incl/Config.hpp"

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
		return (0);
	}

	Server newServer(av[1]);

	newServer.startListen();
	newServer.serverLoop();
	return (0);
}