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
#include "Utils.hpp"

extern volatile sig_atomic_t stopSignal;

class Server
{
public:

	Server(char *av);
	~Server();

	void	startListen(int socket);
	void	serverLoop();
	bool	isServerSocket(int fd);
	void	sendResponse(int client_fd);
	void	closeServer();
	
	int		createConfig(char *av);
	int		checkConfigFile(std::ifstream &conFile);
	void	countPortsInConfig(std::ifstream &configFile);
	void	extractConfigMap(std::ifstream &conFile, std::map<std::string, std::string> &targetMap, std::string target);

	std::map<std::string, std::string>* getConfigMap(const std::string &configName);
	//add exceptions?

	void	extractPorts();
	int		createServerSocket(int port);
	void	portHandler();

	void	make_new_connections(time_t &now, int server_fd);
	void	read_from_connection(time_t &now, std::map<int, std::string> &response_collector, size_t &i);
	void	write_to_connection(std::map<int, std::string> &response_collector, size_t &i);

	class ServerException : std::runtime_error {
	public:
		ServerException(std::string error);
	};

private:
	int									_numPorts;
	std::vector<int>					_ports;
	std::vector<int>					_serverSocket;
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
