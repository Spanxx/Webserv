
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

		// int	parse_request(const std::string &request_raw);
		void	check_headers(const std::string &headers_raw);
		void	splitURI();
		int		split_headers(std::istringstream &rstream);
		bool	parse_chunks(std::string &data, size_t start);
		int		checkURILength();
		int		checkPathChars();
		int		checkRequestedPath();
		int		checkRequestedFiletype();
		
		void	setCode(int code);
		void	setPath(std::string path);
		void	append_body(const std::string &body_part);
		int getCode();
		std::string getMethod();
		std::string getPath();
		std::string getVersion();
		std::string getBody();
		std::string getQuery();
		bool getConnection();
		std::string getHeader(const std::string &key);
		int getContentLength();
		size_t getBodySize();
		bool isChunked();
		

		friend std::ostream &operator<<(std::ostream &os, Request &request); //double check that we're allowed to use friend keyword

	private:
		std::string 						_method;
		std::string 						_path;
		std::string 						_query;
		std::string 						_version;
		std::map<std::string, std::string>	_headers;
		int 								_content_length;
		std::string 						_body;
		int 								_code;
		bool 								_chunked;
		size_t 								_parse_pos;
		Server 								*_server;
};

#endif
