
#include "../incl/Response.hpp"


Response::Response(Request *request): _request(request), _code(request->getCode()) {}

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
		this->handleGET(client_fd);
	else if (_request->getMethod() == "POST")
		this->handlePOST(client_fd);
	else if (_request->getMethod() == "DELETE")
		this->handleDELETE(client_fd);
	else	// other methods or send error method not allowed (405)
		_code = 405; // and process this code
	//std::cout << *this;
}

void	Response::handleGET(int client_fd)
{
	//check path or check in request?
	//extract requested filetype (html, png, jpg..)
	//build response
	std::string response = responseBuilder();

	write(client_fd, response.c_str(), response.length());
}

void	Response::handlePOST(int client_fd)
{
	(void)client_fd;
}

void	Response::handleDELETE(int client_fd)
{
	(void)client_fd;
}


std::string Response::responseBuilder()
{
	std::string response;
	std::string header;
	std::string body;

	// handle body at first, to get content size and type
	body = this->bodyBuilder();
	header = this->headersBuilder();

	response.append(header);
	response.append(body);

	return (response);
}

std::string	Response::headersBuilder()
{
	std::ostringstream header;
	header << this->_request->getVersion() << ' ' 
			<< this->_code << ' ' 
			<< this->_statusPhrase << "\r\n"
			<< this->_request->getPath() << "\r\n"
			<< "Content-Type: text/html\r\n"
			<< "Content-Length: " << this->_headers["Content-Length"] << "\r\n"
			<< "\r\n";	//empty newline to seperate header and body

	return (header.str());
}
std::string	Response::bodyBuilder()
{
	std::string 		line;
	std::string 		body;
	std::string			path;
	int					lineCount = 0;
	std::stringstream	ss;

	path = this->_request->getPath();
	std::cout << "Trying to open: " << path << '\n';
	std::ifstream file(path.c_str());
	if (!file)
	{
		std::cerr << "Requested file not Found\n";
		//load error page 404
		return ("");
	}

	while (getline(file, line))
	{
		body.append(line);
		body.append("\n");
		++lineCount;
	}

	std::cout << "Lines read: " << lineCount << '\n';

	ss << body.size();
	this->_headers["Content-Length"] = ss.str();

	return(body);
}
