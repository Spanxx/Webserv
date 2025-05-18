
#include "../incl/Response.hpp"
#include "../incl/Utils.hpp"

Response::Response(Request *request): _request(request), _code(request->getCode()) 
{
	std::cout << "Response constructed\n";
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
int Response::getCode() { return _code; }

std::string Response::process_request(int client_fd)
{
	this->assign_status_phrase();
	if (_code != 200)
		return handleERROR();
	else if (_request->getMethod() == "GET")
		return handleGET();
	else if (_request->getMethod() == "POST")
		return handlePOST(client_fd);
	else if (_request->getMethod() == "DELETE")
		return handleDELETE(client_fd);
	std::cout << *this->_request << std::endl;
	std::cout << this->_code << " " << this->_status["phrase"] << std::endl;
	//this->sendResponse(client_fd);
	return handleERROR(); //placeholder, check with logc later
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
	std::stringstream ss;
	ss << html.size();
	this->_headers["Content-Length"] = ss.str();
	return html;
}
std::string	Response::handleERROR()
{
	std::string response;
	std::string header;
	std::string body;

	body = this->make_status_page_string();
	header = this->headersBuilder();

	response.append(header);
	response.append(body);
	//std::cout << "Response sent\n";
	//write(client_fd, response.c_str(), response.length());
	return response;
}

std::string	Response::handleGET()
{
	std::string path = this->_request->getPath();
	if (path.size() >= 8 && path.substr(0, 8) == "/cgi-bin")
	{
		std::string exec_path = "." + path;
		std::string query_string = this->_request->getQuery();
		std::cout << "QUERY STRING: " << query_string << std::endl;
		//return cgiExecuter(exec_path, query_string);
		return responseBuilder(); //placeholder
	}
	else
		return responseBuilder();
}

std::string	Response::handlePOST(int client_fd)
{
	(void)client_fd;
	return NULL;
}

std::string	Response::handleDELETE(int client_fd)
{
	(void)client_fd;
	return NULL;
}


std::string Response::responseBuilder()
{
	std::string response;
	std::string header;
	std::string body;

	// handle body at first, to get content size and type
	body = this->bodyBuilder();
	if (body.empty())
		body = this->make_status_page_string();
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
			<< this->_status["phrase"] << "\r\n"
			<< this->_request->getPath() << "\r\n"
			<< "Content-Type: text/html\r\n" //make dynamic
			<< "Content-Length: " << this->_headers["Content-Length"] << "\r\n"
			<< "Connection: keep-alive\r\n" //get from request header
			<< "Location: " << this->_request->getPath() << "\r\n"
			<< "\r\n";	//empty newline to seperate header and body

	return (header.str());
}
std::string	Response::bodyBuilder()
{
	std::string 		line;
	std::string 		body;
	std::string			path;
	std::stringstream	ss;
	int					lineCount = 0;

	path = this->_request->getPath();
	std::cout << "Trying to open: " << path << '\n';
	std::ifstream file(path.c_str());
	if (!file)
	{
		std::cerr << "Requested file open error!\n";
		//load error page 404??
		_code = 404;
		assign_status_phrase();
		return ("");
	}

	while (getline(file, line))
	{
		body.append(line);
		body.append("\n");
		++lineCount;
	}
	
	ss << body.size();

	std::cout << "Lines read: " << lineCount << '\n'
			<< "Chars read: " << ss.str() << std::endl;

	this->_headers["Content-Length"] = ss.str();

	return(body);
}
