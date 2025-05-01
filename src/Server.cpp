
#include "../incl/Server.hpp"

Server::Server(int port) : _port(port)
{
	_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (_serverSocket < 0)
	{
		std::cerr << "Error creating server socket!\n";
		exit(1);
	}

	//prepare server adress structure
	struct sockaddr_in serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;			//IPv4
	serverAddr.sin_addr.s_addr = INADDR_ANY;	//bind to any local adress
	serverAddr.sin_port = htons(_port);			//port in network byte order

	if (bind(_serverSocket, (struct sockaddr *)&serverAddr,sizeof(serverAddr)) < 0)
	{
		std::cerr << "Error binding server socket!\n";
		exit(1);
	}

	std::cout << "Server socket created and bound\n";
}

// Server::Server(Server &other) {}			//a server should not be copyied or assigned to another one.
// Server&	Server::operator=(Server &other) {}	//so we just declare the CC and AC, but not implement them

Server::~Server()
{
	close(_serverSocket);
	std::cout << "Server socket closed\n";
}

void	Server::start()
{
	//start listening for incoming connections
	if (listen(_serverSocket, 1) < 0)
	{
		std::cerr << "Error listening on server socket!\n";
		exit(1);
	}

	std::cout << "Server is listening on port " << _port << "...\n";

	//accept an incoming connection
	struct sockaddr_in	clientAddr;
	socklen_t			clientLen = sizeof(clientAddr);
	int					clientSocket = accept(_serverSocket, (struct sockaddr *)&clientAddr, &clientLen);

	if (clientSocket < 0)
	{
		std::cerr << "Error accepting connection!\n";
		exit(1);
	}

	std::cout << "Client connected!\n";

	//Read data from the client
	char	buffer[1024] = {0};
	int 	n = read(clientSocket, buffer, sizeof(buffer));
	if (n < 0)
	{
		std::cerr << "Error reading from socket!\n";
		exit(1);
	}

	std::cout << "Received request:" << buffer << std::endl;

	//HTTP response
	const char *response = 
		"HTTP/1.1 200 OK\r\n"
		"Content-Type: text/html\r\n"
		"Content-Length: 48\r\n"
		"\r\n"
		"<html><body><h1>Hello from C++ Server!</h1></body></html>";

	//send HTTP response
	send(clientSocket, response, strlen(response), 0);

	close(clientSocket);
}
