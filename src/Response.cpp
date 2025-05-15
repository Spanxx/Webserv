
#include "../incl/Response.hpp"
#include "../incl/Utils.hpp"

Response::Response(Request *request): _request(request), _code(request->getCode())
{
	//status phrase + code here? or set to which default?
}

Response::Response(Response &other)
{
	this->_headers = other._headers;
	this->_body = other._body;
	this->_code = other._code;
	this->_request = other._request;
	this->_status["phrase"] = other._status["phrase"];
	this->_status["code"] = other._status["code"];

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
	this->_status["phrase"] = other._status["phrase"];
	this->_status["code"] = other._status["code"];

	std::cout << "Response assigned\n";

	return (*this);
}



void Response::setCode(int code) { _code = code; }


void Response::process_request(int client_fd)
{
	if (_request->getMethod() == "GET")
		std::cout << "placeholder for processing GET method\n"; //send GET response
	else if (_request->getMethod() == "POST")
		std::cout << "placeholder for processing POST method\n"; //send POST response
	else if (_request->getMethod() == "DELETE")
		std::cout << "placeholder for processing DELETE method\n"; //send DELETE response
	// other methods or send error method not allowed (405)
	else
		_code = 405; // and process this code
	this->assign_status_phrase();
	std::cout << *this->_request << this->_status["phrase"] << std::endl;
	this->sendResponse(client_fd);
}

void Response::assign_status_phrase()
{
	std::string line;
	std::ifstream file("content/data/status_codes.txt");
	if (!file.is_open())
	{
		std::cerr << "Error extracting status phrase: error opening file\n";
		this->_status["phrase"] = "Default";
		return;
	}
	_status["code"] = intToString(_code);
	while (std::getline(file, line))
	{
		if (!line.compare(0, 3, _status["code"])) //compare returns 0 when found matching
		{
			_status["phrase"] = line.substr(6);
			return;
		}
	}
	_status["phrase"] = "Not found";
}



void Response::sendResponse(int client_fd)
{
	std::string response = this->make_status_page_string();
	write(client_fd, response.c_str(), response.length());
}

std::string Response::make_status_page_string()
{
	// read file into string
	std::ifstream file("content/data/status_page.html");
	if (!file)
    	{
		std::cerr << "Error opening status code file\n";
		return NULL;
	}
 	std::stringstream buffer;
	buffer  << file.rdbuf(); //rdbuf to read entire content of file stream into stringstream
	std::string html = buffer.str();

	// 
	replaceAll(html, "{{CODE}}", _status["code"]);
	replaceAll(html, "{{MESSAGE}}", _status["phrase"]);


    	std::ostringstream response_stream;
    	response_stream << "HTTP/1.1 "<< _status["code"] << " " << _status["phrase"] <<"\r\n"
                    << "Content-Type: text/html\r\n"
                    << "Content-Length: " << html.length() << "\r\n"
                    << "\r\n"
                    << html;

	return response_stream.str();
}
