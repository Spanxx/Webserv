
#include	"../incl/Server.hpp"
#include	"../incl/Request.hpp"
#include	"../incl/Response.hpp"
// #include	<sys/time.h>

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

void	Server::extractPorts()
{
	int port;
	std::string item;

	std::map<std::string, std::string> *config = getConfigMap("serverConfig");
	
	if (!config)
		throw ServerException("Extracting serverConfig map failed!");

	std::map<std::string, std::string>::iterator it = config->begin();
	while (it != config->end())
	{
		if (it->first.find("ports") != std::string::npos)
		{	
			std::stringstream ss(it->second);

			while (std::getline(ss, item, ','))
			{
				port = strToInt(item);
				this->_ports.push_back(port);
			}
		}
		++it;
	}

	std::vector<int>::iterator iPorts = this->_ports.begin();
	while (iPorts != this->_ports.end())
	{
		std::cout << "Port: " << *iPorts << '\n';	// add  check for port duplicates
		++iPorts;
	}
}

Server::Server(char *av)
{
	try
	{
		if (this->createConfig(av) == 1)
			throw ServerException("Creating Config failed!");

		this->extractPorts();

		std::vector<int>::iterator it = this->_ports.begin();

		while (it != this->_ports.end())	// maybe change to for with index
		{
			int sock = this->createServerSocket(*it);
			this->_serverSocket.push_back(sock);
			startListen(sock);
			std::cout << "Server socket fd: " << sock << " created and bound\n";
			++it;
		}
	}
	catch (std::exception &e)
	{
		std::cout << "Server exception: " << e.what() << std::endl;
		return ;
	}
}

// Server::Server(Server &other) {}			//a server should not be copyied or assigned to another one.
// Server&	Server::operator=(Server &other) {}	//so we just declare the CC and AC, but not implement them

Server::~Server()
{
	for (size_t i = 0; i < this->_serverSocket.size(); ++i)
	{
		close(this->_serverSocket[i]);
		std::cout << "Server socket fd: " << this->_serverSocket[i] << " closed\n";
	}
}

void	Server::startListen(int socket)
{
	//start listening for incoming connections
	if (listen(socket, 10) < 0)	// was 1, 10 is to test / amount of connections
	{
		throw ServerException("Listen failed!");
		// std::cerr << "Error listening on server socket!\n";
		// exit(1);
	}

	std::cout << "Server starts listening for incomming connections on FD " << socket <<  "\n";

	struct pollfd serverFd;
	serverFd.fd = socket;
	serverFd.events = POLLIN;	// wait for input
	this->_socketArray.push_back(serverFd);
}

// void	Server::startListen()
// {
// 	//start listening for incoming connections
// 	if (listen(this->_serverSocket, 1) < 0)
// 	{
// 		std::cerr << "Error listening on server socket!\n";
// 		exit(1);
// 	}

// 	std::cout << "Server starts listening for incomming connections on FD " << _serverSocket <<  "\n";

// 	struct pollfd serverFd;
// 	serverFd.fd = _serverSocket;
// 	serverFd.events = POLLIN;	// wait for input
// 	this->_socketArray.push_back(serverFd);
// }

void	Server::extractConfigMap(std::ifstream &conFile, std::map<std::string, std::string> &targetMap, std::string target)
{
	std::string	line;

	while (std::getline(conFile, line))
	{
		if (line == target)
		{
			while (std::getline(conFile, line))
			{
				if (line == "}")
				{
					conFile.clear();                // Clear any error flags
					conFile.seekg(0, std::ios::beg); // Go back to the beginning	
					return ;
				}

				if (line.empty() || line[0] == '#')
					continue;
				
				size_t	equalPos = line.find('=');
				if (equalPos != std::string::npos)
				{
					std::string	key = line.substr(0, equalPos);
					std::string value = line.substr(equalPos + 1);
					// std::cout << "config key: " << key << " || value: " << value << '\n';	debug for config values
					//check for key duplicates
					if (targetMap.find(key) != targetMap.end())
					{
						std::cout << "Warning: Duplicate key found " << key << '\n'	// check how nginx handles it
								<< "new Value don't overwrites existing Value!\n";
					}
					else
						targetMap[key] = value;
				}
			}
		}
	}
}

int	Server::checkConfigFile(std::ifstream &conFile)
{
	std::string	line;
	while(std::getline(conFile, line))
	{
		for(int i = 0; line[i] != '\n' && line[i]; ++i)
		{
			if (line[0] == '#')
				break ;
			if (line[i] == 32)	//space
			{
				std::cerr << "Error: Forbidden char <space> found in config file!\n";
				return (1);
			}
			if (line[i] == '=' && line[i + 1] == '-')	//negative value found //  must be changed for new version 111,222,333
			{
				std::cerr << "Error: Non-positive value found in config file!\n";
				return (1);
			}
		}
		// maybe add more checks?
	}
	conFile.clear();                // Clear any error flags
	conFile.seekg(0, std::ios::beg); // Go back to the beginning
	return (0);
}

// void	Server::countPortsInConfig(std::ifstream &conFile)
// {
// 	int	num = 0;
// 	std::string line;

// 	while (std::getline(conFile, line))
// 	{
// 		if (line.find("port") != std::string::npos && line[0] != '#')
// 			++num;
// 	}

// 	this->_numPorts = num;
// 	std::cout << "Number of Ports to listen: " << num << '\n';
// }

int	Server::createConfig(char *av)
{
	std::string filePath = checkFilePath(av);

	// std::ifstream configFile(av);
	std::ifstream configFile(filePath.c_str());
	if (!configFile)
	{
		std::cout << "Reading config File failed!\n";
		return (1);
	}
	if (configFile.peek() == std::ifstream::traits_type::eof())
	{
		std::cout << "The Config file is empty!\n";
		return (1);
	}

	if (Server::checkConfigFile(configFile) == 1)
		return (1);

	//countPortsInConfig(configFile);
	
	this->extractConfigMap(configFile, _serverConfig, "server{");
	this->extractConfigMap(configFile, _dirConfig, "dir{");
	this->extractConfigMap(configFile, _pageConfig, "pages{");
	this->extractConfigMap(configFile, _fileConfig, "files{");
	this->extractConfigMap(configFile, _filetypeConfig, "filetypes{");

	//add a port if there is no specified in the config
	// if (this->_numPorts == 0)
	// {
	// 	std::cerr << "No port specified in config, default is used: 8080\n";
	// 	this->_serverConfig["port1"] = "8080";
	// }

	return (0);
}

void Server::closeServer()
{
	for (size_t i = 0; i < _socketArray.size(); ++i) {
		std::cout << "Closing socket fd " << _socketArray[i].fd << std::endl;
		close(_socketArray[i].fd);
	}
}

std::map<std::string, std::string>* Server::getConfigMap(const std::string &configName)
{
	if (configName == "serverConfig")
		return(&this->_serverConfig);
	if (configName == "dirConfig")
		return(&this->_dirConfig);
	if (configName == "pageConfig")
		return(&this->_pageConfig);
	if (configName == "fileConfig")
		return(&this->_fileConfig);
	if (configName == "filetypeConfig")
		return(&this->_filetypeConfig);
	else
		return (NULL);
}

Server::ServerException::ServerException(const std::string &error) : std::runtime_error(error) {}
