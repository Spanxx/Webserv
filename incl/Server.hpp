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
#include <arpa/inet.h> // need for inet_aton
#include "Utils.hpp"
#include "Libraries.hpp"

extern volatile sig_atomic_t stopSignal;

#define TIMEOUT_SEC 5 //timeout for CGI
#define SEND_COMPLETE 1
#define SEND_ERROR 2
#define SEND_CONTINUE 3

class Request;

class Server
{
public:

	// Server(char *av);
	Server(std::string &serverConfig);
	~Server();

	void	startListen(int socket);
	bool	isServerSocket(int fd);
	void	sendResponse(int client_fd);
	void	closeServer();

	void		createConfig(std::string &serverConfig);
	int		checkConfigFile(std::ifstream &conFile);
	void	extractConfigMap(std::string &configFile, std::map<std::string, std::string> &targetMap, std::string target);
	void	allowedMethods(std::string &trimmed);
	void	createDirStructure();
	void	loadMimeTypes();

	void	storeServerConfig();
	void	extractPorts(std::map<std::string, std::string>::iterator &it);
	void	checkPortDuplicates(int &port);
	void	extractMaxBodySize();
	void	printPorts();

	int	router();

	std::map<std::string, std::string>* getConfigMap(const std::string &configName);

	std::map<std::string, std::string> getUploadDir();
	std::string getErrorPage();
	void	checkScriptsExecutable();

	void	extractVariables();
	void	extractPorts(const std::string &ports);
	void	extractHost(const std::string &host);
	void	extractName(const std::string &name);
	void	extractMaxBody(const std::string &maxbody);
	void	extractErrorPage(const std::string &path);
	void	checkCompletes();
	void	doesRootExist(std::map<std::string, std::string> &targetMap);

	int		createServerSocket(int port);


	std::vector<int>	makeNewConnections(int server_fd);
	bool readFromConnection(std::map<int, std::string> &response_collector, int fd, std::map<int, bool> &keepAlive, std::vector<struct pollfd> &globalPollFds);
	void initialize_request(int fd, const std::string &data, size_t header_end);
	RequestState handleRequest(std::string &data, size_t header_end, std::map<int, bool> &keepAlive, int fd);
	void prepare_response(int fd, std::map<int, std::string> &response_collector);
	int	 write_to_connection(std::map<int, std::string> &response_collector, int fd, std::vector<struct pollfd> &globalPollFds);
	void close_erase(int fd);
	const std::vector<struct pollfd>& getpollFdArray() const;
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

	class ConfigException : public std::runtime_error {
		public:
			ConfigException(const std::string &error);
	};

private:
	std::string 											_name;
	int														_numPorts;
	std::vector<int>										_ports;
	std::string												_IPHost;
	std::vector<int>										_serverSockets; // vector of fd of each socket of the server
	size_t													_maxBodySize;
	std::map<int, std::string> 								_socketBuffers;
	std::map<int, Request*> 								_requestCollector;
	std::vector<struct pollfd>								_pollFdArray; // (_socketArray) pollfd array of each server socket Maybe not necessary anymore

	//std::string																			_host;
	std::map<int, time_t> 																_lastActive;
	std::map<std::string, std::string>													_serverConfig;
	std::map<std::string, std::string>													_dirConfig;
	std::map<std::string, std::map<std::string, std::string> >							_locationBlocks;
	std::map<std::string, std::string>													_mimetypeConfig;
	std::map<std::map<std::string, std::string>, std::map<std::string, std::string> >	_serverMap;
	// std::map<std::map<std::string, std::string>, std::map<std::string, std::string> >	_serverMap;
	std::map<std::string, std::string>	_uploadDir;
	std::string	_errorPage;

	Server(Server &other);
	Server& operator=(Server &other);
};

void	createConfigList(std::string configPath, std::vector<std::string> &configList);

int		mkdir_p(const std::string fullPath, mode_t mode);


#endif //SERVER_HPP
