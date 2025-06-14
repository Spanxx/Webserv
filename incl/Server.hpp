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
#include	<arpa/inet.h> // need for inet_aton
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

	int	router();
	
	std::map<std::string, std::string>* getConfigMap(const std::string &configName);

	std::map<std::string, std::string> getUploadDir();

	void	extractPorts();
	void	extractHost();
	void	extractName();
	int		createServerSocket(int port);


	//void	make_new_connections(time_t &now, int server_fd);
std::vector<int>	make_new_connections(time_t &now, int server_fd, std::vector<struct pollfd> &globalPollFds, std::map<int, time_t> &lastActive);
	//void	read_from_connection(time_t &now, std::map<int, std::string> &response_collector, size_t &i, std::map<int, bool> &keepAlive);
	void	read_from_connection(time_t &now, std::map<int, std::string> &response_collector, int fd, std::map<int, bool> &keepAlive, std::vector<struct pollfd> &globalPollFds, std::map<int, time_t> &lastActive);
	void	initialize_request(int fd, const std::string &data, size_t header_end);
	//void	handle_request(std::string &data, size_t header_end, std::map<int, std::string> &response_collector, std::map<int, bool> &keepAlive, size_t &i);
	void	handle_request(std::string &data, size_t header_end, std::map<int, std::string> &response_collector, std::map<int, bool> &keepAlive, int fd, std::vector<struct pollfd> &globalPollFds, std::map<int, time_t> &lastActive);
	//void	prepare_response(Request *request, std::map<int, std::string> &response_collector, int &i);
	void prepare_response(int fd, std::map<int, std::string> &response_collector);
	//void	write_to_connection(std::map<int, std::string> &response_collector, size_t &i, std::map<int, bool> &keepAlive);
	void write_to_connection(std::map<int, std::string> &response_collector, int fd, std::map<int, bool> &keepAlive, std::vector<struct pollfd> &globalPollFds, std::map<int, time_t> &lastActive);
	//void	close_erase(std::map<int, std::string> &response_collector, size_t &i, std::map<int, bool> &keepAlive);
	void	close_erase(std::map<int, std::string> &response_collector, int fd, std::map<int, bool> &keepAlive, std::vector<struct pollfd> &globalPollFds, std::map<int, time_t> &lastActive);
	void close_erase_fd(int fd, std::map<int, std::string> &response_collector, std::map<int, bool> &keepAlive, std::vector<struct pollfd> &globalPollFds, std::map<int, time_t> &lastActive);
	const std::vector<struct pollfd>& getSocketArray() const;
	const std::vector<int>& getServerSockets() const;

	size_t														getMaxBodySize();
	std::string													getName();
	std::map<std::string, std::map<std::string, std::string> >*	getLocationBlocks();
	void assignUploadDir();
	bool checkPOST(std::map<std::string, std::string> configblock);
	std::string findRoot(std::map<std::string, std::string> configblock);

	class ServerException : public std::runtime_error {
	public:
		ServerException(const std::string &error);
	};

private:
	std::string 											_name;
	int														_numPorts;
	std::vector<int>										_ports;
	std::string												_IPHost;
	std::vector<int>										_serverSocket; // vector of fd of each socket of the server
	size_t													_maxBodySize;
	std::map<int, std::string> 								_socketBuffers;
	std::map<int, Request*> 								_requestCollector;
	std::vector<struct pollfd>								_socketArray; // pollfd array of each server socket

	//std::string																			_host;
	std::map<int, time_t> 																_lastActive;
	std::map<std::string, std::string>													_serverConfig;
	std::map<std::string, std::string>													_dirConfig;
	std::map<std::string, std::map<std::string, std::string> >							_locationBlocks;
	std::map<std::string, std::string>													_mimetypeConfig;
	std::map<std::map<std::string, std::string>, std::map<std::string, std::string> >	_serverMap;
	// std::map<std::map<std::string, std::string>, std::map<std::string, std::string> >	_serverMap;
	std::map<std::string, std::string>	_uploadDir;

	Server(Server &other);
	Server& operator=(Server &other);
};

void	createConfigList(std::string configPath, std::vector<std::string> &configList);

int		mkdir_p(const std::string fullPath, mode_t mode);


#endif //SERVER_HPP
