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

#include "Config.hpp"

class Server
{
public:

	Server(int port);
	~Server();

	void	start();

private:
	int			_port;
	int			_serverSocket;

	Server(Server &other);
	Server& operator=(Server &other);
	
};


#endif //SERVER_HPP