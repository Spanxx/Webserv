
#include "../incl/Server.hpp"

Server::Server(Config *conf)
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
	serverAddr.sin_port = htons(conf->getPort());			//port in network byte order

	int yes = 1;
	setsockopt(this->_serverSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

	if (bind(_serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
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

void	Server::serverLoop()
{
	int time = 50000;
	while (true)
	{
		int ret = poll(_socketArray.data(), _socketArray.size(), time);
		// check ret == 0 for timeout and if we need to close manually or poll does by itself
		if (ret < 0)
		{
			std::cerr << "poll error\n";
			continue;
		} if (ret == 0) {
			std::cout << "Poll timeout " << ret << std::endl;
		}
		for (size_t i = 0; i < _socketArray.size(); ++i)
		{
			if (_socketArray[i].fd == _serverSocket && (_socketArray[i].revents & POLLIN)) //return a non-zero value if the POLLIN bit is set
			{
				struct sockaddr_in	clientAddr;
				socklen_t			clientLen = sizeof(clientAddr);
				int					clientSocket = accept(_serverSocket, (struct sockaddr *)&clientAddr, &clientLen);
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
				this->_socketArray.push_back(clientFd);
				std::cout << "New connection accepted: fd = " << clientFd.fd << std::endl;
			}
			else if (_socketArray[i].revents & POLLIN)
			{
				char	buffer[1024] = {0};
				int 	n = read(_socketArray[i].fd, buffer, sizeof(buffer));
				if (n < 0)
				{
					std::cerr << "Error reading from socket!\n";
					//exit(1);
					continue;
				}
				else if (n == 0) // when EOF is reached, so connection is closed by client
				{
					std::cout << "client disconnected: fd = " << _socketArray[i].fd << std::endl;
					close(_socketArray[i].fd);
					_socketArray.erase(_socketArray.begin() + i); //erases and automatically shifts all later elements one forward
					--i;
					continue;
				}
				std::cout << "Received request:" << buffer << std::endl;
				//HTTP response
				sendResponse(_socketArray[i].fd);

				/*  // this below needs to be expanded and checked later when we have parsing, to make different handlers for keep-alive or not and timeout etc.
				bool keepAlive = true;
				if (!keepAlive)
				{
					std::cout << "no kep-alive connection, closing connection: fd " << _socketArray[i].fd << std::endl;
					close(_socketArray[i].fd);
					_socketArray.erase(_socketArray.begin() + i); //erases and automatically shifts all later elements one forward
					--i;
				}*/
			}

		}

	}
}

void	Server::startListen()
{
	//start listening for incoming connections
	if (listen(this->_serverSocket, 1) < 0)
	{
		std::cerr << "Error listening on server socket!\n";
		exit(1);
	}

	std::cout << "Server starts listening for incomming connections \n";

	struct pollfd serverFd;
	serverFd.fd = _serverSocket;
	serverFd.events = POLLIN;	// wait for input
	this->_socketArray.push_back(serverFd);
	// //accept an incoming connection
	// struct sockaddr_in	clientAddr;
	// socklen_t			clientLen = sizeof(clientAddr);
	// int					clientSocket = accept(_serverSocket, (struct sockaddr *)&clientAddr, &clientLen);

	// if (clientSocket < 0)
	// {
	// 	std::cerr << "Error accepting connection!\n";
	// 	exit(1);
	// }

	// std::cout << "Client connected!\n";

	// //Read data from the client
	// char	buffer[1024] = {0};
	// int 	n = read(clientSocket, buffer, sizeof(buffer));
	// if (n < 0)
	// {
	// 	std::cerr << "Error reading from socket!\n";
	// 	exit(1);
	// }

	// std::cout << "Received request:" << buffer << std::endl;

	// //HTTP response
	// const char *response =
	// 	"HTTP/1.1 200 OK\r\n"
	// 	"Content-Type: text/html\r\n"
	// 	"Content-Length: 48\r\n"
	// 	"\r\n"
	// 	"<html><body><h1>Hello from C++ Server!</h1></body></html>";

	// //send HTTP response
	// send(clientSocket, response, strlen(response), 0);

	// close(clientSocket);
}

void Server::sendResponse(int client_fd) {
    std::ostringstream html;
    html << "<html><body><h1>Hello from C++ Server!</h1>"
         << "<p>Your socket fd is: " << client_fd << "</p>"
         << "</body></html>";

    std::string body = html.str();

    std::ostringstream response_stream;
    response_stream << "HTTP/1.1 200 OK\r\n"
                    << "Content-Type: text/html\r\n"
                    << "Content-Length: " << body.length() << "\r\n"
                    << "\r\n"
                    << body;

    std::string response = response_stream.str();

    write(client_fd, response.c_str(), response.length());
}
