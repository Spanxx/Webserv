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
			throw ServerException("Port duplicate found!");

		++it;
	}
}

int	Server::createServerSocket(int port)
{
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0)
		throw ServerException("Creating server socket failed!");

	//prepare server adress structure
	struct sockaddr_in serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;				//IPv4
	serverAddr.sin_port = htons(port);				//port in network byte order
	if (this->_IPHost == "0.0.0.0") // Dynamic IP address
		serverAddr.sin_addr.s_addr = htonl(INADDR_ANY); // Listen on all available interfaces
	else
	{
		if (inet_aton(this->_IPHost.c_str(), &serverAddr.sin_addr) == 0)
		{
			std::cerr << "Error: Invalid IP address " << this->_IPHost << " for " << this->_name << std::endl;
			close(sock);
			return -1;
		}
	}

	int yes = 1;
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));	// reuse port
	fcntl(sock, F_SETFL, O_NONBLOCK);	// set socket to non-blocking mode
	if (bind(sock, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
	{
		 std::cerr << "Error binding socket to port " << ntohs(serverAddr.sin_port)
						<< ": " << strerror(errno) << " (errno: " << errno << ")" << std::endl;
		throw ServerException("Bind failed!");
		// std::cerr << "Error binding server socket!\n";
		// exit(1);
	}
	return (sock);
}
