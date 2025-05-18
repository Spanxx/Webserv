
#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>
#include <map>
#include <iostream>
#include <sstream>
#include <vector>
#include <cctype>	//needed for std::isalnum
#include "Server.hpp"

class Request
{
	public:
		Request(Server *server);
		Request(Request &other);
		~Request();
		Request& operator=(Request &other);

		int	parse_request(const std::string &request_raw);
		void	splitURI();
		int	parse_headers(std::istringstream &rstream);
		int	checkPathChars();
		int	checkRequestedPath();
		int	checkRequestedFiletype();
		
		void setCode(int code);
		int getCode();
		std::string getMethod();
		std::string getPath();
		std::string getVersion();
		std::string getBody();
		std::string getQuery();

		friend std::ostream &operator<<(std::ostream &os, Request &request); //double check that we're allowed to use friend keyword

	private:
		std::string _method;
		std::string _path;
		std::string _query;
		std::string _version;
		std::map<std::string, std::string> _headers;
		std::string _body;
		int _code;
		Server 		*_server;
};

#endif
