
#include "../incl/Request.hpp"
#include "../incl/Utils.hpp"

Request::Request(Server *server) : _server(server)
{
	std::cout << "Request constructed\n";
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

void	Request::check_headers(const std::string &headers_raw)
{
	std::istringstream rstream(headers_raw); //turn string into stream so it can be read line by line with getline
	std::string line;
	std::cout << "HEADERS RAW: " << headers_raw << "UNTIL HERE" << std::endl;
	if (std::getline(rstream, line))
	{
		std::istringstream lstream(line); //splits with space as delimiter
		std::string extra;
		if (!(lstream >> _method >> _path >> _version) || lstream >> extra) // less than or more than 3 parts
			// return 400;
			this->_code = 400;
	}
	// make extra check for header too long for buffer --> code 431
	// URI to long
	_path = urlDecode(_path);
	if (_path.empty())
	{
		this->_code = 400;
		return;
	}
	splitURI();
	std::cout << "PATH: " << _path << ", QUERY: " << _query << std::endl;

	if (split_headers(rstream) == 1)
		return ;
	if (checkURILength() == 1)
		return ;
	if (checkPathChars() == 1)
		return;
	if (checkRequestedPath() == 1)
		return ;
	if (checkRequestedFiletype() == 1)
		return ;
	this->_code = 200;
}

int Request::split_headers(std::istringstream &rstream)
{
	std::string line;
	bool blank = false;
	_content_length = -1;
	while (std::getline(rstream, line))
	{
		if (!line.empty() && line[line.size() - 1] == '\r') //getline removes \n but not \r 
			line.erase(line.size() - 1); //remove last char (trailing carriage return '\r')
		if (line.empty())	//if there is no body, then there is maybe no empty line
		{
			blank = true;
			break;
		}
		size_t pos = line.find(": ");
		if (pos == std::string::npos || line[pos - 1] == ' ') //if no colon or extra whitespace
			return 0;
		std::string key = line.substr(0, pos);
		std::string value = line.substr(pos + 2);
		trim(key);
		trim(value);
		// std::cout << "key: " << key << " // value: " << value << '\n';	//header values for debugging
		if (key.empty() || value.empty())
			return 0;
		_headers[key] = value;
		if (key == "Content-Length")
			_content_length = std::atoi(value.c_str());
	}

	if (!blank)
	{
		this->_code = 400;
		return (1); //no empty line after header
	}
	if (_content_length == -1 && _method != "POST") //check if only in GET and DELETE it's ok not to have content_length set 
		_content_length = 0;
	return 0;
}


std::ostream &operator<<(std::ostream &os, Request &request)
{
	os << "Method: " << request._method << ", URI: " << request._path << ", VERSION: " << request._version << std::endl;
	os << "Headers:\n";
	for (std::map<std::string, std::string>::const_iterator it = request._headers.begin(); it != request._headers.end(); ++it) {
		os << "  " << it->first << ": " << it->second << "\n";
	    }
	    os << "Body:\n" << request._body << "\n";
	//os << "CODE: " << request._code << std::endl;
	return os;
}


void Request::setCode(int code) { _code = code; }

int	Request::getCode() { return _code; }
std::string Request::getMethod() { return _method; }
std::string Request::getPath() { return _path; }
std::string Request::getVersion() { return _version; }
std::string Request::getBody() { return _body; }
std::string Request::getQuery() { return _query; }

bool Request::getConnection()
{ 
	if (_headers["Connection"] == "keep-alive")
		return true;
	return false;
}

void	Request::append_body(const std::string &body_part)
{
	_body += body_part;
}

std::string Request::getHeader(const std::string &key) 
{ 
	std::map<std::string, std::string>::const_iterator it = _headers.find(key);
	if (it != _headers.end())
        	return it->second;
	std::cout << "Header " << key << " does not exist\n";
	return ""; 
}

int Request::getContentLength() { return _content_length; }

void	Request::splitURI()
{
	size_t pos = _path.find('?');
	if (pos == std::string::npos)
		_query = "";
	else
	{
		_query = _path.substr(pos + 1);
		_path = _path.substr(0, pos);
	}
	
}

void	Request::setPath(std::string path)
{
	this->_path = path;
}