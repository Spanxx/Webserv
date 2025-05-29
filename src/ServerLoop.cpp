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

	if (n <= 0)
	{
		if (n < 0)
			std::cerr << "Error reading from socket!\n";
		else	//if there is nothing to read, it goes in the statement????
			std::cout << "[Browser closed] Client fd " << _socketArray[i].fd << " is closed!" << std::endl;
		close_erase(response_collector, i, keepAlive);
		return;
	}		

	_lastActive[_socketArray[i].fd] = now;
	_socketBuffers[_socketArray[i].fd] += std::string(buffer, n);

	std::string &data = _socketBuffers[_socketArray[i].fd];
	size_t header_end = data.find("\r\n\r\n");
	if (header_end == std::string::npos) //still no header end 
		return;
	// Extract headers and check Content-Length
	if (_requestCollector.find(_socketArray[i].fd) == _requestCollector.end()) //no entry made yet for this request
		initialize_request(_socketArray[i].fd, data, header_end);
	handle_request(data, header_end, response_collector, keepAlive, i);
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
	std::map<int, Request*>::iterator it = _requestCollector.find(_socketArray[i].fd);
	if (it != _requestCollector.end())
	{
		delete it->second;
		_requestCollector.erase(it);
	}
}

void Server::initialize_request(int fd, const std::string &data, size_t header_end)
{
	std::string header_part = data.substr(0, header_end + 4);
	std::cout << "Request from client fd " << fd << std::endl;
		Request *request = new Request(this);
		request->check_headers(header_part);
		//keepAlive[_socketArray[i].fd] = request->getConnection();
		_requestCollector[fd] = request;
}

void Server::handle_request(const std::string &data, size_t header_end, std::map<int, std::string> &response_collector, std::map<int, bool> &keepAlive, size_t &i)
{
	Request *request = _requestCollector[_socketArray[i].fd];
	int content_length = request->getContentLength();
	if (content_length < 0)
	{
		std::cerr << "Missing or invalid Content-Length\n";
		close_erase(response_collector, i, keepAlive);
		return;
	}
	size_t total_required = header_end + 4 + content_length;
	if (content_length == 0 || data.size() >= total_required)
	{
		std::string body_part;
		if (content_length > 0)
			body_part = data.substr(header_end + 4, content_length);
		request->append_body(body_part);
		keepAlive[_socketArray[i].fd] = request->getConnection();
		
		Response *response = new Response(request);
		response_collector[_socketArray[i].fd] = response->process_request(_socketArray[i].fd); 
		_socketArray[i].events = POLLOUT; //switch to writing
		std::cout << "Switched to POLLOUT\n";

		delete request;
		delete response;
		_requestCollector.erase(_socketArray[i].fd);
		_socketBuffers.erase(_socketArray[i].fd);
	}
}
