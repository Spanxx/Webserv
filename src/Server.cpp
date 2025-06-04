
#include	"../incl/Server.hpp"
#include	"../incl/Request.hpp"
#include	"../incl/Response.hpp"
// #include	<sys/time.h>

Server::Server(char *av)
{
	try
	{
		if (this->createConfig(av) == 1)
			throw ServerException("Creating Config failed!");

		this->extractPorts();

		//set default port if none in config file
		if (this->_numPorts == 0)
		{
			std::cout << "No port in config file, default set to 8080!\n";
			this->_ports.push_back(8080);
		}

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

// Copy and assignment operator
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

void Server::closeServer()
{
	for (size_t i = 0; i < _socketArray.size(); ++i) {
		std::cout << "Closing socket fd " << _socketArray[i].fd << std::endl;
		close(_socketArray[i].fd);
	}
}

Server::ServerException::ServerException(const std::string &error) : std::runtime_error(error) {}

size_t	Server::getMaxBodySize() { return _maxBodySize; }
