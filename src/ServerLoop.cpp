#include	"../incl/Server.hpp"
#include	"../incl/Request.hpp"
#include	"../incl/Response.hpp"
#include	"../incl/libraries.hpp"

bool Server::isServerSocket(int fd)
{
	for (size_t i = 0; i < this->_serverSockets.size(); ++i)
	{
		if (this->_serverSockets[i] == fd)
			return true;
	}
	return false;
}

std::vector<int>	Server::makeNewConnections(int server_fd)
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
		newClients.push_back(clientSocket);
		std::cout << "New connection accepted: fd = " << clientSocket << std::endl;
	}
	return newClients;
}

bool	Server::readFromConnection(std::map<int, std::string> &response_collector,
																int fd,
																std::map<int, bool> &keepAlive,
																std::vector<struct pollfd> &globalPollFds)
{
	char buffer[BUFFER_SIZE] = {0};
	ssize_t n = recv(fd, buffer, sizeof(buffer), 0); // recv it's better for sockets

	if (n <= 0)
	{
		if (n < 0)
			std::cerr << "Error reading from socket!\n";
		else
			std::cout << "[Browser closed] Client fd " << fd << " is closed!" << std::endl;
		cleanupConnection(fd);
		return false;
	}

	std::cout << "Read " << n << " bytes from fd " << fd << std::endl;
	 _socketBuffers[fd] += std::string(buffer, n);

	std::string &data = _socketBuffers[fd];

	size_t header_end = data.find("\r\n\r\n");
	if (header_end == std::string::npos)
		return true;

	if (_requestCollector.find(fd) == _requestCollector.end())
		initialize_request(fd, data, header_end);
	RequestState state = handleRequest(data, header_end, keepAlive, fd);

    if (state == REQUEST_INCOMPLETE) {
        // Request is not fully received yet (waiting for more body/chunks)
        // Do NOT prepare response or switch to POLLOUT.
        return true; // Keep connection open, continue polling for read events
    }
    else if (state == REQUEST_COMPLETE || state == REQUEST_ERROR_RESPOND) {
        // Request is fully processed (either successfully, or with an error that warrants a response)

        // *** IMPORTANT ***
        // After processing a complete request, you MUST consume the bytes
        // from _socketBuffers[fd] that belong to this request.
        // Otherwise, they will remain in the buffer and potentially be
        // re-processed or interfere with subsequent requests (pipelining).
        size_t total_consumed_bytes = header_end + 4; // At least headers + CRLFCRLF
        if (!_requestCollector[fd]->isChunked()) {
             total_consumed_bytes += _requestCollector[fd]->getContentLength();
        } else {
            // For chunked, you need to know how many bytes parse_chunks consumed from 'data'
            // This needs to be tracked within your Request object or parse_chunks function
            // For now, let's assume parse_chunks consumed all necessary data for a COMPLETE state
            // If parse_chunks modified 'data' directly (by removing parsed chunks),
            // this part might be different.
        }
        _socketBuffers[fd] = data.substr(total_consumed_bytes); // Consume the processed part

        prepare_response(fd, response_collector); // Prepare the HTTP response (and trigger CGI if needed)

        // Switch socket to POLLOUT to allow sending the response
        for (size_t i = 0; i < globalPollFds.size(); ++i)
        {
            if (globalPollFds[i].fd == fd)
            {
                globalPollFds[i].events = POLLOUT;
                std::cout << "Switched fd " << fd << " to POLLOUT\n";
                break;
            }
        }
        return true; // Indicate that this connection is still alive (now for writing)
    }
    else { // state == REQUEST_ERROR_CLOSE_CONN (e.g., severe parsing error)
        std::cout << "[FATAL REQUEST ERROR] Closing connection for fd " << fd << "." << std::endl;
        cleanupConnection(fd); // Server cleans its own state
        return false; // Signal to Cluster to remove this fd
    }
}

void	Server::write_to_connection(std::map<int, std::string> &response_collector,
															int fd,
															std::map<int, bool> &keepAlive,
															std::vector<struct pollfd> &globalPollFds)
{
	std::string &resp = response_collector[fd];

	std::cout << "Writing response to fd " << fd << ": " << resp.size() << " bytes\n";
	ssize_t sent = send(fd, resp.c_str(), resp.size(), 0);
	if (sent < 0)
	{
		std::cerr << "Send error on fd " << fd << std::endl;
		close_erase(fd);
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
			close_erase(fd);
    }
}

void	Server::close_erase(int fd)
{
	close(fd);

	std::map<int, Request*>::iterator it = _requestCollector.find(fd);
	if (it != _requestCollector.end())
	{
		delete it->second;
		_requestCollector.erase(it);
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
	for (std::vector<struct pollfd>::iterator it = _pollFdArray.begin(); it != _pollFdArray.end(); ++it)
	{
		if (it->fd == fd)
		{
			_pollFdArray.erase(it);
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

RequestState Server::handleRequest(std::string &data, size_t header_end, std::map<int, bool> &keepAlive, int fd)
{
	Request *request = _requestCollector[fd];
	keepAlive[fd] = request->getConnection();

	if (!request->isChunked())
	{
		int content_length = request->getContentLength();
		if (content_length < 0)
		{
			std::cerr << "Missing or invalid Content-Length for fd " << fd << "\n";
			request->setCode(400); // Bad Request
			// close connection!?
			return REQUEST_ERROR_RESPOND; // Return true so prepare_response can set 400 response
		}

		size_t total_required = header_end + 4 + content_length; // +4 for "\r\n\r\n"
		if (data.size() < total_required)
		{
			std::cout << "Not enough data yet for fd " << fd << ", waiting for more...\n";
			return REQUEST_INCOMPLETE; // Keep connection open, still waiting for data
		}
		std::string body_part;
		if (content_length > 0)
			body_part = data.substr(header_end + 4, content_length);

		request->append_body(body_part);

		if (request->getBodySize() > _maxBodySize)
			request->setCode(413);
		return REQUEST_COMPLETE; // Request complete, response can be prepared (even if it's an error response)
	}
	else // It's a chunked request
	{
		size_t chunk_start = header_end + 4; // Start of chunked body after header
		if (request->parse_chunks(data, chunk_start))
		{
			if (request->getBodySize() > _maxBodySize)
				request->setCode(413); // Payload Too Large, even if chunk parsing was successful
			return REQUEST_COMPLETE; // Request complete, response can be prepared
		}
		else if (request->getCode() != 200) // Error during chunk parsing
			return REQUEST_ERROR_RESPOND; // e.g., 400 Bad Request due to chunking error
		return REQUEST_INCOMPLETE; // Still waiting for more chunks
	}
}

void Server::prepare_response(int fd, std::map<int, std::string> &response_collector)
{
	Request *request = _requestCollector[fd];
	Response *response = new Response(request, this->_name);

	response_collector[fd] = response->process_request(fd);

	// Switch to POLLOUT on this fd Hacerlo en cluster!!
	for (size_t j = 0; j < _pollFdArray.size(); ++j)
	{
		if (_pollFdArray[j].fd == fd)
		{
			_pollFdArray[j].events = POLLOUT;
			break;
		}
	}
	std::cout << "Switched to POLLOUT for fd = " << fd << "\n";

	delete request;
	delete response;
	cleanupConnection(fd);
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
		if (!is_valid_hex(data.substr(pos, end - pos), chunk_size))
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
