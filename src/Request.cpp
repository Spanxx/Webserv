
#include "../incl/Request.hpp"

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

int Request::parse_request(const std::string &request_raw)
{
	std::istringstream rstream(request_raw); //turn string into stream so it can be read line by line with getline
	std::string line;
	std::cout << "Request raw:\n" << request_raw << std::endl;
	if (std::getline(rstream, line))
	{
		std::istringstream lstream(line); //splits with space as delimiter
		std::string extra;
		if (!(lstream >> _method >> _path >> _version) || lstream >> extra) // less than or more than 3 parts
			return 400;
	}
	// make extra check for header too long for buffer --> code 431
	if (!parse_headers(rstream))
		return (400);
	if (checkPathChars() == 1)
		return (400);
	if (checkRequestedPath() == 1)
		return (400);
	if (checkRequestedFiletype() == 1)
		return (400);
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
	while (std::getline(rstream, line)) // && line != "\r") //headers, until \r (empty line = end of header section) - getline removes \n but not \r 
	{
		if (!line.empty() && line[line.size() - 1] == '\r')
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
		if (key.empty() || value.empty())	// || key != trim(key) || value != trim(value)) //if there is extra whitespace
			return 0;
		_headers[key] = value;
	}
	if (!blank)
		return 0; //no empty line after header
	return 1;
}

std::string Request::trim(const std::string &str)
{
	size_t start = str.find_first_not_of(" \t\r\n");
	size_t end = str.find_last_not_of(" \t\r\n");
	if (start == std::string::npos || end == std::string::npos)
		return "";
	return str.substr(start, end - start + 1);
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

int	Request::checkPathChars()
{
	const std::string allowedChars = "-_./~";	//inclued A-Z a-Z 0-9 (checked with isalnum())
	const std::string reservedChars = "!*'();:@&=+$,/?#";
	const std::string unsafeChars = "`<>\"{}";

	for (int i = 0; this->_path[i]; ++i)
	{
		//allowed chars, don't need to be unencoded
		if (std::isalnum(this->_path[i]) || allowedChars.find(this->_path[i]) != std::string::npos)
			continue;
		//Reserved	! * ' ( ) ; : @ & = + $ , / ? # (allowed but context-sensitive)
		else if (reservedChars.find(this->_path[i]) != std::string::npos)
			continue;
		else
			return (1);
	}
	return (0);
}

int	Request::checkRequestedPath()
{
	//check if path is dir --> send to mainpage
	if (*(this->_path.end() - 1) == '/')
	{
		std::cout << "Client is trying to request a directory --> redirect to content/index.html\n";
		this->_path = "content/index.html";
		this->_code = 301;
		return (0);
	}

	//add content dir in front of path
	std::string newPath;
	//check for favicon and add content
	if (this->_path == "/favicon.ico")
		newPath = "content/files" + this->_path;
	else
		newPath = "content" + this->_path;
	this->_path = newPath;

	//check with dir in config
	std::map<std::string, std::string> *dirs = this->_server->getConfigMap("dirConfig");
	if (!dirs)
	{
		std::cerr << "Config map 'dirConfig' not found!\n";
		return (1);
	}

	std::cout << "Map pointer: " << dirs << "\n";
	std::cout << "Map size: " << dirs->size() << "\n";

	std::map<std::string, std::string>::iterator it = dirs->begin();
	int	lastSlash = this->_path.find_last_of("/");
	std::string dirString = this->_path.substr(0, lastSlash);
	
	std::cout << dirString << '\n';

	while (it != dirs->end())
	{
		if (dirString == it->second)
		{
			std::cout << "Valid dir found in config file\n";
			break;
		}
		else
			++it;
	}
	if (it == dirs->end())
	{
		std::cout << "Invalid dir requested!\n";
		//reset map iterator
		it = dirs->begin();
		return (1);
	}

	return (0);
}

int	Request::checkRequestedFiletype()
{
	std::map<std::string, std::string> *fileTypes = this->_server->getConfigMap("fileTypesConfig");
	if (!fileTypes)
	{
		std::cerr << "Config map 'fileTypesConfig' not found!\n";
		return (1);
	}

	std::map<std::string, std::string>::iterator it = fileTypes->begin();
	int	lastDot = this->_path.find_last_of(".");
	std::string typeString = this->_path.substr(lastDot);
	
	std::cout << typeString << '\n';

	while (it != fileTypes->end())
	{
		if (typeString == it->second)
		{
			std::cout << "Valid dir found in config file\n";
			break;
		}
		else
			++it;
	}
	if (it == fileTypes->end())
	{
		std::cout << "Invalid dir requested!\n";
		it = fileTypes->begin();
		return (1);
	}
	//reset map iterator
	it = fileTypes->begin();
	return (0);
}
