#include "../incl/Server.hpp"
#include "../incl/Libraries.hpp"

// std::vector<std::string>	extractPorts(std::string &line)
// {
// 	size_t						equalPos = line.find_first_of("=");
// 	size_t						commaPos;
// 	std::string					tmpLine;
// 	std::string					port;
// 	std::vector<std::string>	portList;

// 	if (equalPos == std::string::npos)
// 		return (portList);

// 	tmpLine = line.substr(equalPos + 1);
// 	tmpLine = trim(tmpLine);

// 	while (1)
// 	{
// 		commaPos = tmpLine.find_first_of(",");
// 		if (commaPos != std::string::npos)
// 		{
// 			port = tmpLine.substr(0, commaPos);
// 			port = trim(port);
// 			tmpLine = tmpLine.substr(commaPos + 1);			
// 		}
// 		else
// 			port = trim(tmpLine);
		
// 		checkForPortDuplicates(portList);
		
// 		portList.push_back(port);
		
// 		if (commaPos == std::string::npos)
// 			break;
// 	}

// 	return (portList);
// }

// std::string	extractHost(std::string &line)
// {
// 	std::string	host;
// 	int			equalPos = line.find_first_of("=");

// 	host = line.substr(equalPos + 1);
	
// 	return (trim(host));
// }

// int	createAndCheckCombinations(std::string &host, std::vector<std::string> &ports, std::map<std::string, int> *combinations)
// {
// 	int			port;
// 	std::string socketString;
// 	std::vector<std::string>::iterator it = ports.begin();

// 	while (it != ports.end())
// 	{
// 		socketString = host + " | " + *it;
// 		// std::cout << socketString << '\n';
		
// 		if (combinations->find(socketString) != combinations->end())
// 			return (1);

// 		if (safeAtoi(*it, port) == false)
// 			return (1);

// 		if (host == "0.0.0.0")
// 		{
// 			std::map<std::string, int>::iterator it_combinations = combinations->begin();

// 			while (it_combinations != combinations->end())
// 			{
// 				if (it_combinations->second == port)
// 					return (1);
				
// 				++it_combinations;
// 			}
// 		}
		
// 		(*combinations)[socketString] = port;
// 		++it;
// 	}
// 	return (0);
// }

int	checkForPortDuplicates(std::vector<std::string> *ports, std::string &port)
{
	for (size_t i = 0; i < ports->size(); ++i)
	{
		if (ports->at(i) == port)
			return (1);
	}
	return (0);
}

int	extractPorts(std::vector<std::string> *ports, std::string &line)
{
	size_t						equalPos = line.find_first_of("=");
	size_t						commaPos;
	std::string					tmpLine;
	std::string					port;

	if (equalPos == std::string::npos)
		return (1);

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
		
		if (checkForPortDuplicates(ports, port) == 1)
			return 1;
		
		ports->push_back(port);
		
		if (commaPos == std::string::npos)
			break;
	}

	return (0);
}

int	checkforSocketDuplicates(std::vector<std::string> &configList)
{
	std::vector<std::string>	ports;
	std::string					serverConfigs;
	std::string 				line;
	
	std::vector<std::string>::iterator it = configList.begin();
	
	while (it != configList.end())
	{
		serverConfigs = *it;
		std::istringstream			iss(serverConfigs);

		while (getline(iss, line))
		{
			line = trim(line);
			if (line.find("listen") != std::string::npos)
			{
				if (extractPorts(&ports, line) == 1)
					return (1);
			}
		}
		++it;
	}
	return (0);
}



// int		checkforSocketDuplicates(std::vector<std::string> &configList)
// {
// 	std::string					host;
// 	std::vector<std::string>	ports;
// 	int							socketComplete = 0;
// 	std::string					serverConfigs;
// 	std::string 				line;
// 	std::map<std::string, int>	combinations;
	
// 	std::vector<std::string>::iterator it = configList.begin();
	
// 	while (it != configList.end())
// 	{
// 		serverConfigs = *it;
// 		std::istringstream			iss(serverConfigs);

// 		while (getline(iss, line))
// 		{
// 			line = trim(line);
// 			// if (line.find("host") != std::string::npos)
// 			// {
// 			// 	host = extractHost(line);	// configList is one long string...needs splitting -.-
// 			// 	++socketComplete;
// 			// }
// 			if (line.find("listen") != std::string::npos)
// 			{
// 				ports = extractPorts(line);
// 				++socketComplete;
// 			}

// 			// if (socketComplete == 2)
// 			// {
// 			// 	if (createAndCheckCombinations(host, ports, &combinations) == 1)
// 			// 		return (1);
// 			// 	socketComplete = 0;
// 			// }
// 		}
// 		++it;
// 	}

// 	return (0);
// }
