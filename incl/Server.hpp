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
	
	// Config	*conf;
	int		createConfig(char *av);
	int		checkConfigFile(std::ifstream &conFile);
	void	extractConfigMap(std::ifstream &conFile, std::map<std::string, std::string> &targetMap, std::string target);

	std::map<std::string, std::string>* getConfigMap(std::string configName);
	//add exceptions?

private:
	int									_serverSocket;
	std::vector<struct pollfd>			_socketArray;
	std::map<int, time_t> 				_lastActive;
	std::map<std::string, std::string>	_serverConfig;
	std::map<std::string, std::string>	_dirConfig;
	std::map<std::string, std::string>	_pageConfig;
	std::map<std::string, std::string>	_fileConfig;
	std::map<std::string, std::string>	_filetypeConfig;

	Server(Server &other);
	Server& operator=(Server &other);
};


#endif //SERVER_HPP
