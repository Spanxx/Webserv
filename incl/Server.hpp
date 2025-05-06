#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <fstream>
#include <map>
#include <exception>
#include <cstdlib>
#include <string>
#include <cstring>    		// For memset
#include <sys/socket.h>
#include <netinet/in.h>		//needed for handling IP addresses and protocols
#include <unistd.h>   		// For close()
#include <poll.h>
#include <vector>
#include <fcntl.h>
#include <sstream>
#include <csignal>

#include "Config.hpp"

extern volatile sig_atomic_t stopSignal;

class Config;

class Server
{
public:

	Server(char *av);
	~Server();

	void	startListen();
	void	serverLoop();
	void	sendResponse(int client_fd);
	void	closeServer();
	Config	*conf;
	static Config*	createConfig(char *av);

private:
	int			_serverSocket;
	std::vector<struct pollfd> _socketArray;

	Server(Server &other);
	Server& operator=(Server &other);
};


#endif //SERVER_HPP