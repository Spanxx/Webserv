
#include "../incl/Request.hpp"
#include "../incl/Utils.hpp"

Request::Request()
{
	
}

Request::Request(Request &other)
{

	this->_method = other._method;
	this->_path = other._path;
	this->_version = other._version;
	this->_headers = other._headers;
	this->_body = other._body;
	this->_code = other._code;

	std::cout << "Request copied\n";
}

Request::~Request()
{
	std::cout << "Request deconstructed\n";
}

Request& Request::operator=(Request &other)
{
	if (this == &other)
		return (*this);

	this->_method = other._method;
	this->_path = other._path;
	this->_version = other._version;
	this->_headers = other._headers;
	this->_body = other._body;
	this->_code = other._code;

	std::cout << "Request assigned\n";

	return (*this);
}

int Request::parse_request(const std::string &request_raw)
{
	std::istringstream rstream(request_raw); //turn string into stream so it can be read line by line with getline
	std::string line;
	std::cout << "REQUEST RAW: " << request_raw << "UNTIL HERE" << std::endl;
	if (std::getline(rstream, line))
	{
		std::istringstream lstream(line); //splits with space as delimiter
		std::string extra;
		if (!(lstream >> _method >> _path >> _version) || lstream >> extra) // less than or more than 3 parts
			return 400;
	}
	// make extra check for header too long for buffer --> code 431
	if (!parse_headers(rstream))
		return 400;
	std::ostringstream bstream; // body --> if there is no body, this just adds empty string 
	while (std::getline(rstream, line))
		bstream << line << "\n";
	_body = bstream.str();
	return 200;
}

int Request::parse_headers(std::istringstream &rstream)
{
	std::string line;
	bool blank = false;
	while (std::getline(rstream, line))
	{
		if (!line.empty() && line[line.size() - 1] == '\r') //getline removes \n but not \r 
			line.erase(line.size() - 1); //remove last char (trailing carriage return '\r')
		if (line.empty())
		{
			blank = true;
			break;
		}
		size_t pos = line.find(": ");
		if (pos == std::string::npos || line[pos - 1] == ' ') //if no colon or extra whitespace
			return 0;
		std::string key = line.substr(0, pos);
		std::string value = line.substr(pos + 2);
		if (key.empty() || key != trim(key) || value != trim(value)) //if there is extra whitespace
			return 0;
		_headers[key] = value;
	}
	if (!blank)
		return 0; //no empty line after header
	return 1;
}


std::ostream &operator<<(std::ostream &os, Request &request)
{
	os << "Method: " << request._method << ", URI: " << request._path << ", VERSION: " << request._version << std::endl;
	os << "Headers:\n";
	for (std::map<std::string, std::string>::const_iterator it = request._headers.begin(); it != request._headers.end(); ++it) {
		os << "  " << it->first << ": " << it->second << "\n";
	    }
	    os << "Body:\n" << request._body << "\n";
	os << "CODE: " << request._code << std::endl;
	return os;
}


void Request::setCode(int code) { _code = code; }

int	Request::getCode() { return _code; }
std::string Request::getMethod() { return _method; }
std::string Request::getPath() { return _path; }
std::string Request::getVersion() { return _version; }
std::string Request::getBody() { return _body; }
