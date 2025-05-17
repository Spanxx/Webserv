
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
		void 		process_request(int client_fd);
		std::string responseBuilder();
		std::string	headersBuilder();
		std::string	bodyBuilder();

		void	handleERROR(int client_fd);
		void	handleGET(int client_fd);
		void	handlePOST(int client_fd);
		void	handleDELETE(int client_fd);
		void assign_status_phrase();
		std::string make_status_page_string();

		void sendResponse(int client_fd);

	private:
		Request *_request;
		int _code;
		std::map<std::string, std::string> _status;
		std::map<std::string, std::string> _headers;
		std::string _body;
};

#endif //RESPONSE_HPP
