
#include "incl/Server.hpp"

volatile sig_atomic_t stopSignal = 0;

void signalHandler(int signum)
{
	std::cout << "\nReceived signal " << signum << ". Shutting down server.\n";
	stopSignal =  1;
}

void	createConfigList(char *av, std::vector<std::string> &configList)
{
	bool			inServerBlock = false;
	bool			inLocationBlock = false;

	std::string		line;
	std::string		serverConfig;
	std::ifstream	iss(av);
	if (!iss)
	{
		std::cerr << "Counting server failed!\n";
		return;
	}

	while (getline(iss, line))
	{
		if (line.find("#") != std::string::npos || line.empty())
			continue;
		if (!inServerBlock && line.find("server") != std::string::npos)
			inServerBlock = true;
		else if (inServerBlock && line.find("{") != std::string::npos)
			inLocationBlock = true;
		else if (inServerBlock && inLocationBlock && line.find("}") != std::string::npos)
			inLocationBlock = false;
		else if (inServerBlock && !inLocationBlock && line.find("}") != std::string::npos)
		{
			serverConfig.append(line);
			inServerBlock = false;
			configList.push_back(serverConfig);
			serverConfig.clear();
			continue;
		}	
		line += '\n';
		serverConfig.append(line);
	}
}

void	runAllServers(std::vector<Server> &serverList)
{

}

int main(int ac, char **av)
{
	std::signal(SIGINT, signalHandler);
	
	if (ac != 2)
	{
		std::cout << "Please provide a config file [Usage: ./webserv *.conf]\n";
		return (1);
	}

	std::vector<std::string>	configList;
	std::vector<Server>			serverList;

	createConfigList(av[1], configList);

	if (configList.size() < 1)
	{
		std::cerr << "Loading server configuration failed!\n";
		return (1);
	}

	try
	{
		for (size_t i = 0; i < configList.size(); ++i)
		{
			Server newServer(av[1], configList[i]);
			serverList.push_back(newServer);
			// newServer.serverLoop();
		}
		runAllServers(serverList);
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}

	return (0);
}
