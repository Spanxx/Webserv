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
	Server(char *av, std::string &serverConfig);
	~Server();

	void	startListen(int socket);
	void	serverLoop();
	bool	isServerSocket(int fd);
	void	sendResponse(int client_fd);
	void	closeServer();

	int		createConfig(char *av, std::string &serverConfig);
	int		checkConfigFile(std::ifstream &conFile);
	void	extractConfigMap(std::string &configFile, std::map<std::string, std::string> &targetMap, std::string target);

	void	createDirStructure();
	void	mkdir_p(const std::string fullPath, mode_t mode);
	void	loadMimeTypes();

	std::map<std::string, std::string>* getConfigMap(const std::string &configName);

	void	extractPorts();
	void	extractHost();
	void	extractName();
	int		createServerSocket(int port);

	void	make_new_connections(time_t &now, int server_fd);
	void	read_from_connection(time_t &now, std::map<int, std::string> &response_collector, size_t &i, std::map<int, bool> &keepAlive);
	void	initialize_request(int fd, const std::string &data, size_t header_end);
	void	handle_request(std::string &data, size_t header_end, std::map<int, std::string> &response_collector, std::map<int, bool> &keepAlive, size_t &i);
	void	prepare_response(Request *request, std::map<int, std::string> &response_collector, size_t &i);
	void	write_to_connection(std::map<int, std::string> &response_collector, size_t &i, std::map<int, bool> &keepAlive);
	void	close_erase(std::map<int, std::string> &response_collector, size_t &i, std::map<int, bool> &keepAlive);
	const std::vector<struct pollfd>& getSocketArray() const;
	const std::vector<int>& getServerSocket() const;

	class ServerException : public std::runtime_error {
	public:
		ServerException(const std::string &error);
	};

	size_t	getMaxBodySize();

private:
	std::string 														_name;
	int																		_numPorts;
	std::vector<int>												_ports;
	std::string															_IPHost;
	std::vector<int>												_serverSocket; // vector of fd of each server sockets
	size_t																	_maxBodySize;
	std::map<int, std::string> 								_socketBuffers;
	std::map<int, Request*> 								_requestCollector;
	std::vector<struct pollfd>								_socketArray; // pollfd array of each server socket
	std::map<int, time_t> 																_lastActive;
	std::map<std::string, std::string>											_serverConfig;
	std::map<std::string, std::string>											_dirConfig;
	std::map<std::string, std::string>											_mimetypeConfig;
	std::map<std::map<std::string, std::string>, std::map<std::string, std::string> >	_serverMap;
	// std::map<std::string, std::string>	_pageConfig;
	// std::map<std::string, std::string>	_fileConfig;

	Server(Server &other);
	Server& operator=(Server &other);
};


#endif //SERVER_HPP
