
#include "../incl/Request.hpp"
#include "../incl/Utils.hpp"

Request::Request(Server *server) : _content_length(-1), _code(200), _chunked(false), _parse_pos(0), _uploadDir(server->getUploadDir()), 
	_server(server)
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
	this->_content_length = other._content_length;
	this->_chunked = other._chunked;
	this->_parse_pos = other._parse_pos;

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
	this->_content_length = other._content_length;
	this->_chunked = other._chunked;
	this->_parse_pos = other._parse_pos;

	std::cout << "Request assigned\n";

	return (*this);
}

std::ostream &operator<<(std::ostream &os, Request &request)
{
	os << "Method: " << request._method << ", URI: " << request._path << ", VERSION: " << request._version << std::endl;
	os << "Headers:\n";
	for (std::map<std::string, std::string>::const_iterator it = request._headers.begin(); it != request._headers.end(); ++it)
	{
		os << "  " << it->first << ": " << it->second << "\n";
	}
	//os << "Body:\n" << request._body << "\n";
	//os << "CODE: " << request._code << std::endl;
	return os;
}

void 	Request::setCode(int code) { _code = code; }
void	Request::setPath(std::string path) { this->_path = path; }
void	Request::append_body(const std::string &body_part) { _body += body_part; }

int	Request::getCode() { return _code; }
std::string Request::getMethod() { return _method; }
std::string Request::getPath() { return _path; }
std::string Request::getVersion() { return _version; }
std::string Request::getBody() { return _body; }
std::string Request::getQuery() { return _query; }
int 	Request::getContentLength() { return _content_length; }
size_t Request::getParsePos() const { return _parse_pos; }
std::map<std::string, std::string> Request::getUploadDir() { return _uploadDir; }
bool 	Request::getConnection()
{
	if (_headers["Connection"] == "keep-alive")
		return true;
	return false;
}
std::string Request::getHeader(const std::string &key)
{
	std::map<std::string, std::string>::const_iterator it = _headers.find(key);
	if (it != _headers.end())
        	return it->second;
	std::cout << "Header " << key << " does not exist\n";
	return "";
}
size_t Request::getBodySize() { return _body.size(); }
bool Request::isChunked() { return _chunked; }

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

