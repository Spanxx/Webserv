
#include	"../incl/Server.hpp"
#include	"../incl/Request.hpp"
#include	"../incl/Response.hpp"
// #include	<sys/time.h>

Server::Server(char *av)
{
	_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (_serverSocket < 0)
	{
		std::cerr << "Error creating server socket!\n";
		exit(1);
	}
	// conf = createConfig(av);
	if (this->createConfig(av) == 1)
		exit(1);
	// if (!conf)
		// exit(1); // call close?
		
	//transfer the port from map/string to int for serversocket
	int	port;
	std::stringstream ss(_serverConfig["port"]);
	ss >> port;
		
	//prepare server adress structure
	struct sockaddr_in serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;				//IPv4
	serverAddr.sin_addr.s_addr = INADDR_ANY;		//bind to any local adress
	serverAddr.sin_port = htons(port);	//port in network byte order
	// serverAddr.sin_port = htons(conf->getPort());	//port in network byte order

	int yes = 1;
	setsockopt(this->_serverSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

	if (bind(_serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
	{
		std::cerr << "Error binding server socket!\n";
		exit(1);
	}
	// delete conf;
	std::cout << "Server socket created and bound\n";
}

// Server::Server(Server &other) {}			//a server should not be copyied or assigned to another one.
// Server&	Server::operator=(Server &other) {}	//so we just declare the CC and AC, but not implement them

Server::~Server()
{
	close(_serverSocket);
	std::cout << "Server socket closed\n";
}

void	Server::serverLoop()
{
	int pollTimeout = 5000;		//timeout --> checks for new connections (milliseconds)
	int clientTimeout = 50;		//timeout before a client gets disconnected (seconds)

	while (!stopSignal)
	{
		int ret = poll(_socketArray.data(), _socketArray.size(), pollTimeout);
		(void)ret;
		
		time_t now = time(NULL);

		for (size_t i = 0; i < _socketArray.size(); ++i)
		{
			//Timeout check for each client
			if (_socketArray[i].fd != _serverSocket && now - _lastActive[_socketArray[i].fd] > clientTimeout)
			{
				std::cout << "Timeout --> client fd " << _socketArray[i].fd << " is closed!" << std::endl;
				close(_socketArray[i].fd);
				_lastActive.erase(_socketArray[i].fd);
				_socketArray.erase(_socketArray.begin() + i);
				--i;
				continue;
			}
			//checks the server for new connections from clients
			if (_socketArray[i].fd == _serverSocket && (_socketArray[i].revents & POLLIN)) //return a non-zero value if the POLLIN bit is set
			{
				struct sockaddr_in	clientAddr;
				socklen_t		clientLen = sizeof(clientAddr);
				int			clientSocket = accept(_serverSocket, (struct sockaddr *)&clientAddr, &clientLen);
				if (clientSocket < 0)
				{
					std::cerr << "client socket error\n";
					// check if we need to clean or close something
					continue;
				}
				fcntl(clientSocket, F_SETFL, O_NONBLOCK); // check if at some point we need to call fcntl with  FD_CLOEXEC.
				struct pollfd clientFd;
				clientFd.fd = clientSocket;
				clientFd.events = POLLIN;	// wait for input
				clientFd.revents = 0; // initializing this but poll() will handle it
				this->_socketArray.push_back(clientFd);
				_lastActive[clientSocket] = now;
				std::cout << "New connection accepted: fd = " << clientFd.fd << std::endl;
			}
			else if (_socketArray[i].revents & POLLIN)		//handles the client connection
			{
				char	buffer[1024] = {0};
				int 	n = read(_socketArray[i].fd, buffer, sizeof(buffer));

				if (n < 0)
				{
					std::cerr << "Error reading from socket!\n";	//close the socket?
					continue;
				}
				if (n == 0)	//if there is nothing to read, it goes in the statement????
				{
					std::cout << "[Browser closed] Client fd " << _socketArray[i].fd << " is closed!" << std::endl;
					close(_socketArray[i].fd);
					_socketArray.erase(_socketArray.begin() + i);	//erases and automatically shifts all later elements one forward
					--i;
					continue;
				}
				
				_lastActive[_socketArray[i].fd] = time(NULL);

				// std::cout << "Received request:" << buffer << std::endl;
				std::cout << "Request from client fd " << _socketArray[i].fd << std::endl;
				Request *request = new Request(this);
				request->setCode(request->parse_request(buffer)); // set error codes, depending on which the response will be sent
				
				Response *response = new Response(request);
				//HTTP response	
				response->process_request(_socketArray[i].fd); // launch send responde from here later?
				// sendResponse(_socketArray[i].fd); // later maybe remove below, because will be called from inside process request function?

				delete request;
				delete response;
			}
			else if (_socketArray[i].revents & POLLERR || _socketArray[i].revents & POLLHUP || _socketArray[i].revents & POLLNVAL) //closed connection / EOF / error
			{
				std::cout << "REVENTS: client fd " << _socketArray[i].fd << " is closed!" << std::endl;
				close(_socketArray[i].fd);
				_socketArray.erase(_socketArray.begin() + i);	//erases and automatically shifts all later elements one forward
				--i;
				continue;
			}
		}
	}
	closeServer();
}

void	Server::startListen()
{
	//start listening for incoming connections
	if (listen(this->_serverSocket, 1) < 0)
	{
		std::cerr << "Error listening on server socket!\n";
		exit(1);
	}

	std::cout << "Server starts listening for incomming connections on FD " << _serverSocket <<  "\n";

	struct pollfd serverFd;
	serverFd.fd = _serverSocket;
	serverFd.events = POLLIN;	// wait for input
	this->_socketArray.push_back(serverFd);
}

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
					// break;
					return ;
				}

				if (line.empty() || line[0] == '#')
					continue;
				
				size_t	equalPos = line.find('=');
				if (equalPos != std::string::npos)
				{
					std::string	key = line.substr(0, equalPos);
					std::string value = line.substr(equalPos + 1);
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
				std::cout << "Error: Forbidden char <space> found in config file!\n";
				return (1);
			}
		}
		// maybe add more checks?
	}
	conFile.clear();                // Clear any error flags
	conFile.seekg(0, std::ios::beg); // Go back to the beginning
	return (0);
}

int	Server::createConfig(char *av)
{
	std::ifstream configFile(av);
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

	// if (Config::checkConfigFile(configFile) == 1)
	if (Server::checkConfigFile(configFile) == 1)
		return (1);

	// std::map<std::string, std::string> _serverConfig;
	this->extractConfigMap(configFile, _serverConfig, "server{");
	// std::map<std::string, std::string> dirConfig;
	this->extractConfigMap(configFile, _dirConfig, "dir{");
	// std::map<std::string, std::string> pageConfig;
	this->extractConfigMap(configFile, _pageConfig, "pages{");
	// std::map<std::string, std::string> fileConfig;
	this->extractConfigMap(configFile, _fileConfig, "files{");
	// std::map<std::string, std::string> filetypeConfig;
	this->extractConfigMap(configFile, _filetypeConfig, "filetypes{");

	
	// Config *config = new Config(_serverConfig);
	// one class for each map or all together?

	return (0);
}

void Server::closeServer() {
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

Server::ServerException::ServerException(std::string error) : std::runtime_error(error) {}
