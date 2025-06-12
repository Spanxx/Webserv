#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <sys/types.h>
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

#define TIMEOUT_SEC 5 //timeout for CGI

class Request;

class Server
{
public:

	// Server(char *av);
	Server(std::string &serverConfig);
	~Server();

	void	startListen(int socket);
	void	serverLoop();
	bool	isServerSocket(int fd);
	void	sendResponse(int client_fd);
	void	closeServer();
	
	int		createConfig(std::string &serverConfig);
	int		checkConfigFile(std::ifstream &conFile);
	void	extractConfigMap(std::string &configFile, std::map<std::string, std::string> &targetMap, std::string target);
	
	void	createDirStructure();
	// void	mkdir_p(const std::string fullPath, mode_t mode);
	void	loadMimeTypes();

	void	storeServerConfig();
	void	extractPorts(std::map<std::string, std::string>::iterator &it);
	void	checkPortDuplicates(int &port);
	void	extractMaxBodySize();
	void	printPorts();
	int		createServerSocket(int port);

	int	router();
	
	std::map<std::string, std::string>* getConfigMap(const std::string &configName);

	void	make_new_connections(time_t &now, int server_fd);
	void	read_from_connection(time_t &now, std::map<int, std::string> &response_collector, size_t &i, std::map<int, bool> &keepAlive);
	void	initialize_request(int fd, const std::string &data, size_t header_end);
	void	handle_request(std::string &data, size_t header_end, std::map<int, std::string> &response_collector, std::map<int, bool> &keepAlive, size_t &i);
	void	prepare_response(Request *request, std::map<int, std::string> &response_collector, size_t &i);
	void	write_to_connection(std::map<int, std::string> &response_collector, size_t &i, std::map<int, bool> &keepAlive);
	void	close_erase(std::map<int, std::string> &response_collector, size_t &i, std::map<int, bool> &keepAlive);

	size_t														getMaxBodySize();
	std::string													getName();
	std::map<std::string, std::map<std::string, std::string> >*	getLocationBlocks();

	class ServerException : public std::runtime_error {
	public:
		ServerException(const std::string &error);
	};

private:
	std::string																			_name;
	std::string																			_host;
	int																					_numPorts;
	size_t																				_maxBodySize;
	std::vector<int>																	_ports;
	std::vector<int>																	_serverSocket;
	std::vector<struct pollfd>															_socketArray;
	std::map<int, std::string> 															_socketBuffers;
	std::map<int, Request*> 															_requestCollector;
	std::map<int, time_t> 																_lastActive;
	std::map<std::string, std::string>													_serverConfig;
	std::map<std::string, std::string>													_dirConfig;
	std::map<std::string, std::map<std::string, std::string> >							_locationBlocks;
	std::map<std::string, std::string>													_mimetypeConfig;
	// std::map<std::map<std::string, std::string>, std::map<std::string, std::string> >	_serverMap;

	Server(Server &other);
	Server& operator=(Server &other);
};

void	createConfigList(std::string configPath, std::vector<std::string> &configList);

int		mkdir_p(const std::string fullPath, mode_t mode);


#endif //SERVER_HPP
