
#include "incl/Server.hpp"
#include "incl/Libraries.hpp"
#include "incl/Cluster.hpp"

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

std::vector<std::string>	extractPorts(std::string &line)
{
	size_t						equalPos = line.find_first_of("=");
	size_t						commaPos;
	std::string					tmpLine;
	std::string					port;
	std::vector<std::string>	portList;

	if (equalPos == std::string::npos)
		return (portList);

	tmpLine = line.substr(equalPos + 1);
	tmpLine = trim(tmpLine);

	while (1)
	{
		commaPos = tmpLine.find_first_of(",");
		if (commaPos != std::string::npos)
		{
			port = tmpLine.substr(0, commaPos);
			port = trim(port);
			tmpLine = tmpLine.substr(commaPos + 1);			
		}
		else
			port = trim(tmpLine);
		
		portList.push_back(port);
		
		if (commaPos == std::string::npos)
			break;
	}

	return (portList);
}

std::string	extractHost(std::string &line)
{
	std::string	host;
	int			equalPos = line.find_first_of("=");

	host = line.substr(equalPos + 1);
	
	return (trim(host));
}

int	createCombinations(std::string &host, std::vector<std::string> &ports, std::map<std::string, int> *combinations)
{

	(void)host;
	(void)ports;
	// create all combinations of host + port and store this combostring as key in map...
	// check if each key is unique, otherwise we have a duplicate
	// value is just trash (not needed because std::set is not allowed in cpp11)

	return (0);
}

int		checkforSocketDuplicates(std::vector<std::string> &configList)
{
	std::string					host;
	std::vector<std::string>	ports;
	int							socketComplete = 0;
	std::string					serverConfigs;
	std::string 				line;
	std::map<std::string, int>	combinations;
	
	std::vector<std::string>::iterator it = configList.begin();
	
	while (it != configList.end())
	{
		serverConfigs = *it;
		std::istringstream			iss(serverConfigs);

		while (getline(iss, line))
		{
			line = trim(line);
			if (line.find("host") != std::string::npos)
			{
				host = extractHost(line);	// configList is one long string...needs splitting -.-
				++socketComplete;
			}
			else if (line.find("listen") != std::string::npos)
			{
				ports = extractPorts(line);
				++socketComplete;
			}

			if (socketComplete == 2)
			{
				createCombinations(host, ports, &combinations);
				socketComplete = 0;
			}
		}
		++it;
	}
	return (0);
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
	std::vector<Server*>		serverList;
	
	createConfigList(configPath, configList);
	if (configList.size() < 1)
	{
		std::cerr << "Loading server configuration failed!\n";
		return (1);
	}

	if (checkforSocketDuplicates(configList) == 1)
	{
		std::cerr << "Socket duplicate (Host + Port) found!\n";
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
