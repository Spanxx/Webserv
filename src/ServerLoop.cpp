#include	"../incl/Server.hpp"
#include	"../incl/Request.hpp"
#include	"../incl/Response.hpp"

void	Server::portHandler()
{
	
}
bool Server::isServerSocket(int fd)
{
	for (size_t i = 0; i < this->_serverSocket.size(); ++i)
	{
		if (this->_serverSocket[i] == fd)
			return true;
	}
	return false;
}

void	Server::serverLoop()
{
	int pollTimeout = 5000;		//timeout --> checks for new connections (milliseconds)
	int clientTimeout = 50;		//timeout before a client gets disconnected (seconds)
	std::map<int, std::string> response_collector;
	std::map<int, bool> keepAlive;

	for (size_t i = 0; i < this->_serverSocket.size(); ++i)
		fcntl(this->_serverSocket[i], F_SETFL, O_NONBLOCK);
	
	while (!stopSignal)
	{
		int ret = poll(_socketArray.data(), _socketArray.size(), pollTimeout);
		(void)ret;
		
		time_t now = time(NULL);

		for (size_t i = 0; i < _socketArray.size(); ++i)
		{
			//Timeout check for each client
			if (!isServerSocket(_socketArray[i].fd) && now - _lastActive[_socketArray[i].fd] > clientTimeout)
			{
				std::cout << "Timeout --> client fd " << _socketArray[i].fd << " is closed!" << std::endl;
				close_erase(response_collector, i, keepAlive);
				continue;
			}

			if (_socketArray[i].revents & POLLIN)   //return a non-zero value if the POLLIN bit is set	//handles the client connection
				read_from_connection(now, response_collector, i, keepAlive);
			else if (_socketArray[i].revents & POLLOUT)
				write_to_connection(response_collector, i, keepAlive);
			else if (_socketArray[i].revents & POLLERR || _socketArray[i].revents & POLLHUP || _socketArray[i].revents & POLLNVAL) //closed connection / EOF / error
			{
				std::cout << "REVENTS: client fd " << _socketArray[i].fd << " is closed!" << std::endl;
				close_erase(response_collector, i, keepAlive);
				continue;
			}
		}
	}
	closeServer();
}


void Server::make_new_connections(time_t &now, int server_fd)
{
	while (true) //loops over all pending connections and accepts them until there are no more (EAGAIN or EWOULDBLOCK)
	{
		struct sockaddr_in	clientAddr;
		socklen_t			clientLen = sizeof(clientAddr);
		int					clientSocket = accept(server_fd, (struct sockaddr *)&clientAddr, &clientLen);
		
		if (clientSocket < 0)
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK) //there are no more connections to accept right now
				break;
			else
			{
				std::cerr << "Error accepting client connection\n";
				// check if we need to clean or close something
				break;
			}
		}
		fcntl(clientSocket, F_SETFL, O_NONBLOCK); // check if at some point we need to call fcntl with  FD_CLOEXEC.
		struct pollfd clientFd;
		clientFd.fd = clientSocket;
		clientFd.events = POLLIN;	// wait for input
		clientFd.revents = 0; // initializing this but poll() will handle it
		_socketArray.push_back(clientFd);
		_lastActive[clientSocket] = now;
		std::cout << "New connection accepted: fd = " << clientFd.fd << std::endl;
	}
}


void Server::read_from_connection(time_t &now, std::map<int, std::string> &response_collector, size_t &i, std::map<int, bool> &keepAlive)
{
	if (isServerSocket(_socketArray[i].fd))
	{
		make_new_connections(now, this->_socketArray[i].fd);
		return;
	}
	char	buffer[1024] = {0};
	int 	n = read(_socketArray[i].fd, buffer, sizeof(buffer));

	if (n < 0)
	{
		std::cerr << "Error reading from socket!\n";	//close the socket?
		return;
	}
	if (n == 0)	//if there is nothing to read, it goes in the statement????
	{
		std::cout << "[Browser closed] Client fd " << _socketArray[i].fd << " is closed!" << std::endl;
		close_erase(response_collector, i, keepAlive);
		return;
	}
				
	//_lastActive[_socketArray[i].fd] = time(NULL);
	_lastActive[_socketArray[i].fd] = now; //to make all be evaluated to the same instance + save computation power

	// std::cout << "Received request:" << buffer << std::endl;
	std::cout << "Request from client fd " << _socketArray[i].fd << std::endl;
	Request *request = new Request(this);
	request->parse_request(buffer);
	keepAlive[_socketArray[i].fd] = request->getConnection();
	// request->setCode(request->parse_request(buffer)); // set error codes, depending on which the response will be sent
				
	Response *response = new Response(request);
				
	//response->process_request(_socketArray[i].fd);
	response_collector[_socketArray[i].fd] = response->process_request(_socketArray[i].fd); 
	_socketArray[i].events = POLLOUT; //switch to writing
	std::cout << "Switched to POLLOUT\n";

	delete request;
	delete response;
}


void Server::write_to_connection(std::map<int, std::string> &response_collector, size_t &i, std::map<int, bool> &keepAlive)
{
	std::string resp = response_collector[_socketArray[i].fd];

	ssize_t sent = send(_socketArray[i].fd, resp.c_str(), resp.size(), 0);
	if (sent < 0)
	{
		std::cerr << "Send error on fd " << _socketArray[i].fd << std::endl;
		close_erase(response_collector, i, keepAlive);
		return;
	}
	if ((size_t)sent < resp.size())
		response_collector[_socketArray[i].fd] = resp.substr(sent); //cuts off first sent amount of bytes and updates response string
	else
	{
		
		if (keepAlive[_socketArray[i].fd])
		{
			response_collector.erase(_socketArray[i].fd);
			_socketArray[i].events = POLLIN; //switch back to read mode
			std::cout << "Switched to POLLIN\n";
		}
		else
			close_erase(response_collector, i, keepAlive);
	}
}

void	Server::close_erase(std::map<int, std::string> &response_collector, size_t &i, std::map<int, bool> &keepAlive)
{
	close(_socketArray[i].fd);
	_lastActive.erase(_socketArray[i].fd);
	response_collector.erase(_socketArray[i].fd);
	keepAlive.erase(_socketArray[i].fd);
	_socketArray.erase(_socketArray.begin() + i);
	--i;
}
