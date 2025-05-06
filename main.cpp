
#include "incl/Server.hpp" 
#include "incl/Config.hpp"





int main(int ac, char **av)
{
	if (ac != 2)
	{
		std::cout << "Please provide a config file [Usage: ./webserv *.conf]\n";
		return (0);
	}
	// browser: 127.0.0.1:555
	Server newServer;
	newServer.conf =  Server::createConfig(av[1]);
	if (!newServer.conf)
		return(0);
	std::cout << "map port: " << newServer.conf-> getPort() << std::endl;
	newServer.startListen();
	newServer.serverLoop();
	delete newServer.conf;
	return (0);
}