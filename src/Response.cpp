
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

std::string Response::process_request(int client_fd) // Every handler shoudl update _body, _code and the headers are built in the end
{
	this->assign_status_phrase();
	if (_code != 200)
		handleERROR();
	else if (_request->getMethod() == "GET")
		handleGET();
	else if (_request->getMethod() == "POST")
		handlePOST(client_fd);
	else if (_request->getMethod() == "DELETE")
		handleDELETE(client_fd);
	this->header = headersBuilder(); // to have the headerBuilder centralized
	std::cout << *this->_request << std::endl;
	std::cout << this->_code << " " << this->_status["phrase"] << std::endl;
	return responseBuilder(); // //placeholder, check with logc later
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

std::string Response::make_status_page_string(unsigned int code)
{
	if (code)
	{
		_code = code;
		assign_status_phrase();
	}
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

	body = this->make_status_page_string(this->_code);
	//header = this->headersBuilder(); this will be done in the process_request

	response.append(header);
	response.append(body);
	return response;
}

void	Response::handleGET()
{
	std::string uri = this->_request->getPath();
	std::string fileType = getMimeType(uri);
	if (isCGI(uri))
	{
		std::string exec_path = "./" + uri;
		std::cout << "EXEC PATH: " << exec_path << std::endl;
		std::string query_string = this->_request->getQuery();
		std::cout << "QUERY STRING: " << query_string << std::endl;
		cgiExecuter(exec_path, query_string);
		return;
	}
	else
		bodyBuilder();
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
	std::string phrase;
	std::string header;
	std::string body;

	std::string status = this->make_status_page_string(_code);
	// handle body at first, to get content size and type
	header = this->headersBuilder();

	response.append(this->_statusPhrase);
	response.append(header);
	response.append(this->_body);

	std::cout << " --> Response:\n" << response << std::endl;
	return (response);
}

std::string	Response::headersBuilder()
{
	std::ostringstream header;
	_headers["Content-Length"] = std::to_string(_body.size());
	if (_headers.find("Content-Type") == _headers.end())
		_headers["Content-Type"] = "text/plain";
	header << this->_request->getVersion() << ' '
			<< this->_code << ' '
			<< this->_status["phrase"] << "\r\n"
			<< this->_request->getPath() << "\r\n"
			<< "Content-Type:" << this->_headers["Content-type"] <<"\r\n"
			<< "Content-Length: " << this->_headers["Content-Length"] << "\r\n"
			<< "Connection: keep-alive\r\n" //get from request header
			<< "Location: " << this->_request->getPath() << "\r\n"
			<< "\r\n";	//empty newline to seperate header and body

	return (header.str());
}

void	Response::bodyBuilder()
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
		this->_code = 404; //phrase
		return;
		//return make_status_page_string(404);
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

	this->_body = body;
}

std::string Response::getMimeType(const std::string &path) {
	size_t dotPos = path.find_last_of('.');
	if (dotPos == std::string::npos)
		return "application/octet-stream"; // default binary?

	std::string ext = path.substr(dotPos + 1);
	if (ext == "html" || ext == "htm") return "text/html";
	if (ext == "css") return "text/css";
	if (ext == "js") return "application/javascript";
	if (ext == "json") return "application/json";
	if (ext == "png") return "image/png";
	if (ext == "jpg" || ext == "jpeg") return "image/jpeg";
	if (ext == "gif") return "image/gif";
	//we can add more types here
	return "application/octet-stream";
}

bool Response::isCGI(const std::string &path) {
	if (path.find("/cgi-bin/") != std::string::npos) { return true; }
	if (path.find(".cgi") != std::string::npos) { return true; }
	return false;
}