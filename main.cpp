
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

void runAllServers(std::vector<Server*> &serverList)
{
	int pollTimeout = 500;		// milliseconds
	int clientTimeout = 10;		// seconds

	std::map<int, std::string> responseCollector;
	std::map<int, bool> keepAlive;
	std::map<int, time_t> lastActive;

	std::vector<struct pollfd> globalPollFds;
	std::map<int, Server*> socketToServerMap;

	// Initialize globalPollFds and fd -> server map
	for (std::vector<Server*>::iterator it = serverList.begin(); it != serverList.end(); ++it)
	{
		const std::vector<struct pollfd>& serverSockets = (*it)->getSocketArray();
		const std::vector<int>& serverSocketsFd = (*it)->getServerSockets();

		for (size_t i = 0; i < serverSockets.size(); ++i)
		{
			globalPollFds.push_back(serverSockets[i]);
			socketToServerMap[serverSockets[i].fd] = *it;
			fcntl(serverSocketsFd[i], F_SETFL, O_NONBLOCK);
		}
	}

	while (!stopSignal)
	{
		int ret = poll(globalPollFds.data(), globalPollFds.size(), pollTimeout);
		if (ret < 0)
		{
			std::cerr << "Poll error: " << strerror(errno) << std::endl;
			continue;
		}

		time_t now = time(NULL);

		for (size_t i = 0; i < globalPollFds.size(); ++i)
		{
			int fd = globalPollFds[i].fd;
			short revents = globalPollFds[i].revents;

			if (revents == 0)
				continue;

			Server *server = socketToServerMap[fd];
			if (!server)
			{
				std::cerr << "Unknown server for fd " << fd << "\n";
				continue;
			}

			// Timeout handling for clients
			if (!server->isServerSocket(fd) && now - lastActive[fd] > clientTimeout)
			{
				std::cout << "[TIMEOUT] Closing inactive fd " << fd << "\n";
				server->close_erase(responseCollector, fd, keepAlive, globalPollFds, lastActive);
				continue;
			}

			// Accept new connections
			if (server->isServerSocket(fd) && (revents & POLLIN))
			{
				std::vector<int> newClients = server->make_new_connections(now, fd, globalPollFds, lastActive);
				for (size_t j = 0; j < newClients.size(); ++j)
					socketToServerMap[newClients[j]] = server;
			}
			else if (revents & POLLIN)
			{
				try {
					server->read_from_connection(now, responseCollector, fd, keepAlive, globalPollFds, lastActive);
				}
				catch (const std::exception &e) {
					std::cout << "[READ ERROR] Closing fd " << fd << ": " << e.what() << "\n";
					server->close_erase(responseCollector, i, keepAlive, globalPollFds, lastActive);
				}
			}
			else if (revents & POLLOUT)
			{
				server->write_to_connection(responseCollector, i, keepAlive, globalPollFds, lastActive);
			}
			else if (revents & (POLLERR | POLLHUP | POLLNVAL))
			{
				std::cout << "[FD ERROR] Closing fd " << fd << "\n";
				server->close_erase(responseCollector, i, keepAlive, globalPollFds, lastActive);
			}
		}
	}
	for (size_t i = 0; i < serverList.size(); ++i)
	{
		serverList[i]->closeServer();
		std::cout << "Server " << i << " closed.\n";
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
	std::vector<Server*>			serverList;

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
			Server*newServer = new Server(av[1], configList[i]);
			serverList.push_back(newServer);
			// newServer.serverLoop();
		}
		runAllServers(serverList);
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}
	for (size_t i = 0; i < serverList.size(); ++i)
		delete serverList[i]; // Calls the Server's destructor and frees memory
	return (0);
}
