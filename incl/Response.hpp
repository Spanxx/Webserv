
#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <string>
#include <map>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <fstream>
#include <unistd.h>
#include "Request.hpp"

class Response {
	public:
		Response(Request *request);
		Response(Response &other);
		~Response();
		Response& operator=(Response &other);

		void 		setCode(int code);
		int 		getCode();
		std::string 		process_request(int client_fd);
		std::string responseBuilder();
		std::string	headersBuilder();
		std::string	bodyBuilder();

		std::string	handleERROR();
		std::string	handleGET();
		std::string	handlePOST(int client_fd);
		std::string	handleDELETE(int client_fd);
		void assign_status_phrase();
		//std::string make_status_page_string();
		std::string make_status_page_string(unsigned int code);

		void sendResponse(int client_fd);
		std::string cgiExecuter(const std::string &path, const std::string &query);

	private:
		Request *_request;
		int _code;
		std::map<std::string, std::string> _status;
		std::map<std::string, std::string> _headers;
		std::string _body;
};

#endif //RESPONSE_HPP
