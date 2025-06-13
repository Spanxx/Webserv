#include	"../incl/Server.hpp"
#include	"../incl/Request.hpp"
#include	"../incl/Response.hpp"

bool Server::isServerSocket(int fd)
{
	for (size_t i = 0; i < this->_serverSocket.size(); ++i)
	{
		if (this->_serverSocket[i] == fd)
			return true;
	}
	return false;
}

/*void	Server::serverLoop()
{
	int pollTimeout = 500;		//timeout --> checks for new connections (milliseconds)
	int clientTimeout = 1200;		//timeout before a client gets disconnected (seconds)
	std::map<int, std::string> response_collector;
	std::map<int, bool> keepAlive;

	for (size_t i = 0; i < this->_serverSocket.size(); ++i)
		fcntl(this->_serverSocket[i], F_SETFL, O_NONBLOCK);
	std::cout << "MAX BODY SIZE: " << _maxBodySize << std::endl;
	while (!stopSignal)
	{
		int ret = poll(_socketArray.data(), _socketArray.size(), pollTimeout);
		(void)ret;

		time_t now = time(NULL);

		for (size_t i = 0; i < _socketArray.size(); ++i)
		{
			//Timeout check for each client
			if (!isServerSocket(_socketArray[i].fd) && now - lastActive[_socketArray[i].fd] > clientTimeout)
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
	closeServer(); // call close_erase here as well?
}*/


/*void Server::make_new_connections(time_t &now, int server_fd)
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
		lastActive[clientSocket] = now;
		std::cout << "New connection accepted: fd = " << clientFd.fd << std::endl;
	}
}*/

std::vector<int>	Server::make_new_connections(time_t &now, int server_fd, std::vector<struct pollfd> &globalPollFds, std::map<int, time_t> &lastActive)
{
	std::vector<int> newClients;

	while (true)
	{
		struct sockaddr_in clientAddr;
		socklen_t clientLen = sizeof(clientAddr);
		int clientSocket = accept(server_fd, (struct sockaddr *)&clientAddr, &clientLen);

		if (clientSocket < 0)
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK)
				break;
			else
			{
				std::cerr << "Error accepting client connection\n";
				break;
			}
		}

		fcntl(clientSocket, F_SETFL, O_NONBLOCK);

		struct pollfd clientFd;
		clientFd.fd = clientSocket;
		clientFd.events = POLLIN;
		clientFd.revents = 0;

		globalPollFds.push_back(clientFd);
		_socketArray.push_back(clientFd);
		lastActive[clientSocket] = now;

		newClients.push_back(clientSocket);

		std::cout << "New connection accepted: fd = " << clientSocket << std::endl;
	}

	return newClients;
}

/*void Server::read_from_connection(time_t &now, std::map<int, std::string> &response_collector, size_t &i, std::map<int, bool> &keepAlive)
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

	std::cout << "Read " << n << " bytes from fd " << _socketArray[i].fd << std::endl;
	lastActive[_socketArray[i].fd] = now;
	_socketBuffers[_socketArray[i].fd] += std::string(buffer, n);
	std::string &data = _socketBuffers[_socketArray[i].fd];
	size_t header_end = data.find("\r\n\r\n");
	if (header_end == std::string::npos) //still no header end
		return;
	//check if chunked or not, parse body either based on content length or chunks, then process

	if (_requestCollector.find(_socketArray[i].fd) == _requestCollector.end()) //no entry made yet for this request
		initialize_request(_socketArray[i].fd, data, header_end);
	handle_request(data, header_end, response_collector, keepAlive, i);
}*/

void	Server::read_from_connection(time_t &now,
																std::map<int, std::string> &response_collector,
																int fd,
																std::map<int, bool> &keepAlive,
																std::vector<struct pollfd> &globalPollFds,
																std::map<int, time_t> &lastActive)
{
	if (isServerSocket(fd))
	{
		make_new_connections(now, fd, globalPollFds, lastActive);
		return;
	}

	char buffer[1024] = {0};
	int n = read(fd, buffer, sizeof(buffer));

	if (n <= 0)
	{
		if (n < 0)
			std::cerr << "Error reading from socket!\n";
		else
			std::cout << "[Browser closed] Client fd " << fd << " is closed!" << std::endl;

		close_erase(response_collector, fd, keepAlive, globalPollFds, lastActive);
		return;
	}

	std::cout << "Read " << n << " bytes from fd " << fd << std::endl;
	lastActive[fd] = now;
	 _socketBuffers[fd] += std::string(buffer, n);

	std::string &data = _socketBuffers[fd];
	size_t header_end = data.find("\r\n\r\n");
	if (header_end == std::string::npos)
		return;

	if (_requestCollector.find(fd) == _requestCollector.end())
		initialize_request(fd, data, header_end);
	handle_request(data, header_end, response_collector, keepAlive, fd, globalPollFds, lastActive);
}


/*void Server::write_to_connection(std::map<int, std::string> &response_collector, size_t &i, std::map<int, bool> &keepAlive)
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
}*/

void	Server::write_to_connection(std::map<int, std::string> &response_collector,
															int fd,
															std::map<int, bool> &keepAlive,
															std::vector<struct pollfd> &globalPollFds,
															std::map<int, time_t> &lastActive)
{
	std::string &resp = response_collector[fd];

	ssize_t sent = send(fd, resp.c_str(), resp.size(), 0);
	if (sent < 0)
	{
		std::cerr << "Send error on fd " << fd << std::endl;
		close_erase(response_collector, fd, keepAlive, globalPollFds, lastActive);
		return;
	}

	if ((size_t)sent < resp.size())
		resp = resp.substr(sent); // not all sent; store remainder
	else
	{
		if (keepAlive[fd])
		{
			response_collector.erase(fd);
			// Switch to POLLIN
			for (size_t i = 0; i < globalPollFds.size(); ++i)
			{
				if (globalPollFds[i].fd == fd)
				{
					globalPollFds[i].events = POLLIN;
					std::cout << "Switched fd " << fd << " to POLLIN\n";
					break;
				}
			}
		}
		else
			close_erase(response_collector, fd, keepAlive, globalPollFds, lastActive);
    }
}

/*void	Server::close_erase(std::map<int, std::string> &response_collector, size_t &i, std::map<int, bool> &keepAlive)
{
	close(_socketArray[i].fd);
	lastActive.erase(_socketArray[i].fd);
	response_collector.erase(_socketArray[i].fd);
	keepAlive.erase(_socketArray[i].fd);
	std::map<int, Request*>::iterator it = _requestCollector.find(_socketArray[i].fd);
	if (it != _requestCollector.end())
	{
		delete it->second;
		_requestCollector.erase(it);
	}
	_socketArray.erase(_socketArray.begin() + i);
	--i;

}*/

void	Server::close_erase(std::map<int, std::string> &response_collector,
												int fd,
												std::map<int, bool> &keepAlive,
												std::vector<struct pollfd> &globalPollFds,
												std::map<int, time_t> &lastActive)
{
	close(fd);
	lastActive.erase(fd);
	response_collector.erase(fd);
	keepAlive.erase(fd);

	std::map<int, Request*>::iterator it = _requestCollector.find(fd);
	if (it != _requestCollector.end())
	{
		delete it->second;
		_requestCollector.erase(it);
	}

	// Remove from globalPollFds
	for (size_t i = 0; i < globalPollFds.size(); ++i)
	{
		if (globalPollFds[i].fd == fd)
		{
			globalPollFds.erase(globalPollFds.begin() + i);
			break;
		}
	}
}

void Server::close_erase_fd(int fd,
	std::map<int, std::string> &response_collector,
	std::map<int, bool> &keepAlive,
	std::vector<struct pollfd> &globalPollFds,
	std::map<int, time_t> &lastActive)
{
	close(fd);
	// Remove from _socketArray
	for (std::vector<struct pollfd>::iterator it = _socketArray.begin(); it != _socketArray.end(); ++it)
	{
		if (it->fd == fd)
		{
			_socketArray.erase(it);
			break;
		}
	}
	for (std::vector<struct pollfd>::iterator it = globalPollFds.begin(); it != globalPollFds.end(); ++it)
	{
		if (it->fd == fd)
		{
			globalPollFds.erase(it);
			break;
		}
	}
	response_collector.erase(fd);
	keepAlive.erase(fd);
	lastActive.erase(fd);

	std::cout << "[CLOSE] Closed client fd = " << fd << std::endl;
}

void Server::initialize_request(int fd, const std::string &data, size_t header_end)
{
	std::string header_part = data.substr(0, header_end + 4);
	std::cout << "Request from client fd " << fd << std::endl;
	Request *request = new Request(this);
	request->check_headers(header_part); // add check for header size
	_requestCollector[fd] = request;
}

/*void Server::handle_request(std::string &data, size_t header_end, std::map<int, std::string> &response_collector, std::map<int, bool> &keepAlive, size_t &i)
{
	Request *request = _requestCollector[_socketArray[i].fd];
	keepAlive[_socketArray[i].fd] = request->getConnection();
	if (!request->isChunked())
	{
		int content_length = request->getContentLength();
		if (content_length < 0)
		{
			std::cerr << "Missing or invalid Content-Length\n";
			request->setCode(400);
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
			if (request->getBodySize() > _maxBodySize)
				request->setCode(413);
			// keepAlive[_socketArray[i].fd] = request->getConnection();
			prepare_response(request, response_collector, i);
		}
	}
	else
	{
		size_t chunk_start = header_end + 4;
		if (request->parse_chunks(data, chunk_start) || request->getCode() != 200)
			prepare_response(request, response_collector, i);
	}
}*/

void	Server::handle_request(std::string &data, size_t header_end,
													std::map<int, std::string> &response_collector,
													std::map<int, bool> &keepAlive,
													int fd,
													std::vector<struct pollfd> &globalPollFds,
													std::map<int, time_t> &lastActive)
{
	Request *request = _requestCollector[fd];
	keepAlive[fd] = request->getConnection();

	if (!request->isChunked())
	{
		int content_length = request->getContentLength();
		if (content_length < 0)
		{
			std::cerr << "Missing or invalid Content-Length\n";
			request->setCode(400);
			close_erase_fd(fd, response_collector, keepAlive, globalPollFds, lastActive);
			return;
		}
		size_t total_required = header_end + 4 + content_length;
		if (content_length == 0 || data.size() >= total_required)
		{
			std::string body_part;
			if (content_length > 0)
				body_part = data.substr(header_end + 4, content_length);
			request->append_body(body_part);
			if (request->getBodySize() > _maxBodySize)
				request->setCode(413);

			prepare_response(fd, response_collector);

			// Switch to POLLOUT
			for (size_t j = 0; j < globalPollFds.size(); ++j)
			{
				if (globalPollFds[j].fd == fd)
				{
					globalPollFds[j].events = POLLOUT;
					break;
				}
			}
		}
	}
	else
	{
		size_t chunk_start = header_end + 4;
		if (request->parse_chunks(data, chunk_start) || request->getCode() != 200)
		{
			prepare_response(fd, response_collector);

			// Switch to POLLOUT
			for (size_t j = 0; j < globalPollFds.size(); ++j)
			{
				if (globalPollFds[j].fd == fd)
				{
					globalPollFds[j].events = POLLOUT;
					break;
				}
			}
		}
	}
}


/*void Server::prepare_response(Request *request, std::map<int, std::string> &response_collector, size_t &i)
{
	Response *response = new Response(request);
	response_collector[_socketArray[i].fd] = response->process_request(_socketArray[i].fd); 
	_socketArray[i].events = POLLOUT; //switch to writing
	std::cout << "Switched to POLLOUT\n";

	delete request;
	delete response;
	_requestCollector.erase(_socketArray[i].fd);
	_socketBuffers.erase(_socketArray[i].fd);
}*/

void Server::prepare_response(int fd, std::map<int, std::string> &response_collector)
{
	Request *request = _requestCollector[fd];
	Response *response = new Response(request, this->_name);

	response_collector[fd] = response->process_request(fd);

	// Switch to POLLOUT on this fd
	for (size_t j = 0; j < _socketArray.size(); ++j)
	{
		if (_socketArray[j].fd == fd)
		{
			_socketArray[j].events = POLLOUT;
			break;
		}
	}
	std::cout << "Switched to POLLOUT for fd = " << fd << "\n";

	delete request;
	delete response;
	_requestCollector.erase(fd);
	_socketBuffers.erase(fd);
}

bool	Request::parse_chunks(std::string &data, size_t start)
{
	size_t pos;
	if (_parse_pos == 0)
		pos = start;
	else
		pos = _parse_pos;
	while (true)
	{
		size_t end = data.find("\r\n", pos);
		if (end == std::string::npos)
			break;
		int chunk_size = 0;
		if (!isValidHex(data.substr(pos, end - pos), chunk_size))
		{
			std::cerr << "Invalid chunk size\n";
			_code = 400;
			return false;
		}
		size_t chunk_start = end + 2;
		size_t chunk_end = chunk_start + chunk_size;
		if (chunk_size == 0)
		{
			if (data.size() < end + 4 || data.substr(end + 2, 2) != "\r\n")
			{
				std::cerr << "Missing CRLF after final 0 chunk\n";
				_code = 400;
				return false;
			}
			data.erase(0, end + 4);
			_parse_pos = 0;
			return true;

		}
		if (data.find("\r\n", chunk_start) != chunk_end)
		{
			std::cerr << "Chunk data smaller than indicated\n";
			_code = 400;
			return false;
		}
		if (data.size() < chunk_end + 2) // not enough data yet
			break;
		if (data.substr(chunk_end, 2) != "\r\n")
		{
			std::cerr << "Missing CRLF after chunk data\n";
			_code = 400;
			return false;
		}
		_body.append(data.substr(chunk_start, chunk_size));
		if (_body.size() > _server->getMaxBodySize())
		{
			_code = 413;
			return false;
		}

		pos = chunk_end + 2;
	}
	_parse_pos = pos;
	return false;
}
