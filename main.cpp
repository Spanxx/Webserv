
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
	int pollTimeout = 500;		//timeout --> checks for new connections (milliseconds)
	int clientTimeout = 10;		//timeout before a client gets disconnected (seconds)
	std::map<int, std::string> response_collector;
	std::map<int, bool> keepAlive;

	std::vector<struct pollfd>	globalPollFds;
	std::map<int, Server*> socketToServerMap;

	for(std::vector<Server>::iterator it = serverList.begin(); it != serverList.end(); ++it)
	{
		const std::vector<struct pollfd>& serverSockets = it->getSocketArray(); // get the pollfd array of each server
		for (size_t i = 0; i < serverSockets.size(); ++i) // then for each server's pollfd array we are pushing it to the global array, and also mapping the fd to the server object
		{
			globalPollFds.push_back(serverSockets[i]);
			socketToServerMap[serverSockets[i].fd] = &(*it);
		}
		const std::vector<int> &serverSockets = it->getServerSockets();
		for (size_t i = 0; i < serverSockets.size(); ++i)
			fcntl(serverSockets[i], F_SETFL, O_NONBLOCK);
	}
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
