#include	"../incl/Server.hpp"
#include	"../incl/Request.hpp"
#include	"../incl/Response.hpp"

void	Server::serverLoop()
{
	int pollTimeout = 5000;		//timeout --> checks for new connections (milliseconds)
	int clientTimeout = 50;		//timeout before a client gets disconnected (seconds)
	std::map<int, std::string> response_collector;

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
					_lastActive.erase(_socketArray[i].fd);
					response_collector.erase(_socketArray[i].fd);
					_socketArray.erase(_socketArray.begin() + i);	//erases and automatically shifts all later elements one forward
					--i;
					continue;
				}
				
				//_lastActive[_socketArray[i].fd] = time(NULL);
				_lastActive[_socketArray[i].fd] = now; //to make all be evaluated to the same instance + save computation power

				// std::cout << "Received request:" << buffer << std::endl;
				std::cout << "Request from client fd " << _socketArray[i].fd << std::endl;
				Request *request = new Request(this);
				request->setCode(request->parse_request(buffer)); // set error codes, depending on which the response will be sent
				
				Response *response = new Response(request);
				//HTTP response	
				
				//response->process_request(_socketArray[i].fd); // launch send responde from here later?
				response_collector[_socketArray[i].fd] = response->process_request(_socketArray[i].fd); 
				_socketArray[i].events = POLLOUT; //switch to writing
				std::cout << "Switched to POLLOUT\n";

				delete request;
				delete response;
			}
			else if (_socketArray[i].revents & POLLOUT)
			{
				std::string resp = response_collector[_socketArray[i].fd];
				ssize_t sent = send(_socketArray[i].fd, resp.c_str(), resp.size(), 0);
				if (sent < 0)
				{
					std::cerr << "Send error on fd " << _socketArray[i].fd << std::endl;
					close(_socketArray[i].fd);
					_lastActive.erase(_socketArray[i].fd);
					response_collector.erase(_socketArray[i].fd);
					_socketArray.erase(_socketArray.begin() + i);	//erases and automatically shifts all later elements one forward
					--i;
					continue;
				}
				if ((size_t)sent < resp.size())
					resp = resp.substr(sent); //partial send
				else
				{
					response_collector.erase(_socketArray[i].fd);
					_socketArray[i].events = POLLIN; //switch back to read mode
					std::cout << "Switched to POLLIN\n";
				}
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
