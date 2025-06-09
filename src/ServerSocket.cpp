#include	"../incl/Server.hpp"
#include	"../incl/Request.hpp"
#include	"../incl/Response.hpp"

void	Server::printPorts()
{
	std::vector<int>::iterator it = this->_ports.begin();
	
	while (it != this->_ports.end())
	{
		std::cout << "Port: " << *it << '\n';
		++it;
	}
}

void	Server::checkPortDuplicates(int &port)
{
	std::vector<int>::iterator it = this->_ports.begin();

	while (it != this->_ports.end())
	{
		if (port == *it)
			throw ServerException("Port dupblicate found!");

		++it;
	}
}

void	Server::extractPorts(std::map<std::string, std::string>::iterator &it)
{
	int			port = 0;
	int			portCounter;
	std::string item;
	std::string	trimmedItem;
	
	std::istringstream iss(it->second);
	while (getline(iss, item, ','))
	{
		trimmedItem = trim(item);
		port = strToInt(trimmedItem);
		checkPortDuplicates(port);
		this->_ports.push_back(port);
		++portCounter;
	}
	this->_numPorts = portCounter;
}

void	Server::storeServerConfig()
{
	std::map<std::string, std::string> *config = getConfigMap("serverConfig");
	
	if (!config)
		throw ServerException("Extracting serverConfig map failed!");

	std::map<std::string, std::string>::iterator it = config->begin();
	while (it != config->end())
	{
		if (it->first.find("listen") != std::string::npos)
			extractPorts(it);
		
		if (it->first.find("name") != std::string::npos)
			this->_name = it->second;
		
		if (it->first.find("maxbodysize") != std::string::npos)
			_maxBodySize = atoi(it->second.c_str());

		++it;
	}

	printPorts();
}

int	Server::createServerSocket(int port)
{
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0)
	{
		throw ServerException("Creating server socket failed!");
	}

	//prepare server adress structure
	struct sockaddr_in serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;				//IPv4
	serverAddr.sin_addr.s_addr = INADDR_ANY;		//bind to any local adress
	serverAddr.sin_port = htons(port);				//port in network byte order

	int yes = 1;
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));	// reuse port

	if (bind(sock, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
	{
		throw ServerException("Bind failed!");
		// std::cerr << "Error binding server socket!\n";
		// exit(1);
	}
	return (sock);
}