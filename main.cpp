
#include "incl/Server.hpp"
#include "incl/Libraries.hpp"
#include "incl/Cluster.hpp"
#include "incl/Utils.hpp"

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
		std::cerr << "Invalid path for config file!\n"
				<< "Usage: ./webserv server.conf\n";
		return (1);
	}

	std::vector<std::string>	configList;
	std::vector<Server*>		serverList;
	
	try
	{
		createConfigList(configPath, configList);
		if (configList.size() < 1)
		{
			throw std::runtime_error("Loading server configuration failed!");
			// std::cerr << "Loading server configuration failed!\n";
			// return (1);
		}
		
		if (checkforSocketDuplicates(configList) == 1)
		{
			throw std::runtime_error("Duplicate Port found!\nServer creation aborted.");
			// std::cerr << "Socket duplicate (Host + Port) found!\n"
			// << "Server creation aborted.\n";
			// return (1);
		}
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return (1);
	}

	Cluster cluster;

	try
	{
		cluster.initializeServers(configList);
		cluster.run();
	}
	catch (std::exception &e)
	{
		std::cerr << "Error during server initialization or execution: ";
		std::cerr << e.what() << std::endl;
	}
	for (size_t i = 0; i < serverList.size(); ++i)
		delete serverList[i]; // Calls the Server's destructor and frees memory
	return (0);
}
