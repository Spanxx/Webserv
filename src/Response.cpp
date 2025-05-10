
#include "../incl/Response.hpp"

Response::Response(Request *request): _request(request), _code(request->getCode())
{
	
}

Response::Response(Response &other)
{
	this->_headers = other._headers;
	this->_body = other._body;
	this->_code = other._code;
	this->_request = other._request;
	this->_statusPhrase = other._statusPhrase;

	std::cout << "Response copied\n";
}

Response::~Response()
{
	std::cout << "Response deconstructed\n";
}

Response& Response::operator=(Response &other)
{
	if (this == &other)
		return (*this);
	this->_headers = other._headers;
	this->_body = other._body;
	this->_code = other._code;
	this->_request = other._request;
	this->_statusPhrase = other._statusPhrase;

	std::cout << "Response assigned\n";

	return (*this);
}



void Response::setCode(int code) { _code = code; }


void Response::process_request(int client_fd)
{
	(void)client_fd; // to be removed later, just to avoid unused variable error 
	if (_request->getMethod() == "GET")
		std::cout << "placeholder for processing GET method\n"; //send GET response
	else if (_request->getMethod() == "POST")
		std::cout << "placeholder for processing POST method\n"; //send POST response
	else if (_request->getMethod() == "DELETE")
		std::cout << "placeholder for processing DELETE method\n"; //send DELETE response
	// other methods or send error method not allowed (405)
	else
		_code = 405; // and process this code
	//std::cout << *this;
}