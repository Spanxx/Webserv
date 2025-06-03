
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
#include <limits.h>
#include "Request.hpp"
#include "Utils.hpp"

class Response {
	public:
		Response(Request *request);
		Response(Response &other);
		~Response();
		Response& operator=(Response &other);

		void 		setCode(int code);
		int 		getCode();
		std::string process_request(int client_fd);
		std::string responseBuilder();
		std::string	headersBuilder();
		void		bodyBuilder();

		void		handleERROR(int statusCode);
		void		handleGET();
		void		handlePOST();
		void		handleDELETE();
		void 		assign_status_phrase();
		//std::string make_status_page_string();
		//void make_status_page_string(unsigned int code);

		void 		sendResponse(int client_fd);
		void 		cgiExecuter(std::string path, const std::string &query);
		void 		parseCGIOutput(const std::string &output);
		std::string getMimeType(const std::string &path);
		bool 		isCGI(const std::string &path);
		bool		isUploadsDir(const std::string &path);

	private:
		Request *_request;
		int _code;
		std::map<std::string, std::string> _status;
		std::map<std::string, std::string> _headers;
		std::string _body;
};

#endif //RESPONSE_HPP
