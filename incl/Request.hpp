
#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>
#include <map>
#include <iostream>
#include <sstream>
#include <vector>

class Request
{
	public:
		Request();
		Request(Request &other);
		~Request();
		Request& operator=(Request &other);

		int	parse_request(const std::string &request_raw);
		int	parse_headers(std::istringstream &rstream);
		int	checkRequestedPath();
		int	checkRequestedFiletype();

		
		void setCode(int code);
		int getCode();
		std::string getMethod();
		std::string getPath();
		std::string getVersion();
		std::string getBody();
		static std::string trim(const std::string &str);

		friend std::ostream &operator<<(std::ostream &os, Request &request); //double check that we're allowed to use friend keyword

	private:
		std::string _method;
		std::string _path;
		std::string _version;
		std::map<std::string, std::string> _headers;
		std::string _body;
		int _code;
};

#endif
