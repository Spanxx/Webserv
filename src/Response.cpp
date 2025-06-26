
#include "../incl/Response.hpp"
#include "../incl/Utils.hpp"

Response::Response(Request *request, std::string &hostName): _request(request), _code(request->getCode())
{
	std::cout << "Response constructed\n";
	this->_headers["hostname"] = hostName;
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
	(void)client_fd;
	this->assign_status_phrase();
	if (_code != 200)
		handleERROR(this->_code);
	else if (_request->getMethod() == "GET")
		handleGET();
	else if (_request->getMethod() == "POST")
		handlePOST();
	else if (_request->getMethod() == "DELETE")
		handleDELETE();
	std::cout << *this->_request << std::endl;
	std::cout << this->_code << " " << this->_status["phrase"] << std::endl;
	return responseBuilder();
}

void Response::assign_status_phrase()
{
	std::string line;
	std::ifstream file("www/error/status_codes.txt");
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
	file.close();
	_status["phrase"] = "Not found";
}

void	Response::handleERROR(int statusCode)
{
	this->_code = statusCode;
	assign_status_phrase();
	// read file into string

	std::ifstream file(_request->getErrorPage().c_str());
	if (!file.is_open())
	{
		std::cerr << "Error opening status code file\n";
		return;
	}
 	std::stringstream buffer;
	buffer  << file.rdbuf(); //rdbuf to read entire content of file stream into stringstream
	file.close();
	std::string html = buffer.str();

	replaceAll(html, "{{CODE}}", _status["code"]);
	replaceAll(html, "{{MESSAGE}}", _status["phrase"]);
	std::stringstream ss;
	ss << html.size();
	this->_headers["Content-Length"] = ss.str();
	this->_headers["Content-Type"] = "text/html";
	this->_body = html;
	//return html;
}

void	Response::handleGET()
{
	std::string uri = this->_request->getPath();
	std::string fileType = getMimeType(uri);
	this->_headers["Content-Type"] = fileType;
	std::cout << "Response File type: " << fileType << std::endl;
	if (isCGI(uri))
	{
		// std::string exec_path = "./" + uri;
		std::string exec_path = uri;
		std::cout << "EXEC PATH: " << exec_path << std::endl;
		std::string query_string = this->_request->getQuery();
		std::cout << "QUERY STRING: " << query_string << std::endl;
		cgiExecuter(exec_path, query_string);
	}
	else
		bodyBuilder();
}

void	Response::handlePOST()
{
	std::string uri = this->_request->getPath();
	std::string fileType = getMimeType(uri);
	this->_headers["Content-Type"] = fileType;
	std::cout << "Response File type: " << fileType << std::endl;
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
		POSTBodyBuilder();
}

void	Response::handleDELETE()
{
	std::string uri = this->_request->getPath();
		if (access(uri.c_str(), F_OK) != 0)
		{
			handleERROR(404);
			return;
		}
		if (std::remove(uri.c_str()) != 0)
		{
			handleERROR(500);
			return;
		}
		this->setCode(200);
		this->_headers["Content-Length"] = "0";
}

std::string Response::responseBuilder()
{
	std::string response;

	// handle body at first, to get content size and type
	response.append(this->headersBuilder());
	response.append(this->_body);

	// if (this->_request->getPath() != "www/files/favicon.ico")
	// 	std::cout << " --> Response:\n" << response << std::endl;
	//std::cout << "Response body: "<< std::endl << this->_body << "-- End of body --"<<std::endl;
	return (response);
}

std::string	Response::headersBuilder()
{
	std::ostringstream header;

	if (_headers.find("Content-Type") == _headers.end())
		_headers["Content-Type"] = "text/html";	// should we change these to text/html for the error pages

	header << this->_request->getVersion() << ' '
			<< this->_code << ' '
			<< this->_status["phrase"] << "\r\n"
			// << this->_request->getPath() << "\r\n"							// needed?
			<< "Host: " << this->_headers["hostname"] << "\r\n"										// shall we keep it, nessessary for webhosting (multiple clients share one server to host there page)
			<< "Connection: " << this->_request->getHeader("Connection") << "\r\n"
			<< "Content-Type: " << this->_headers["Content-Type"] <<"\r\n"
			<< "Content-Length: " << atoi(this->_headers["Content-Length"].c_str()) << "\r\n";
			if (this->_code >= 300 && this->_code < 400)
				header << "Location: " << this->_request->getPath() << "\r\n";
			header << "\r\n";	//empty newline to seperate header and body

	// std::cout << "Location for redir: " << this->_request->getPath() << '\n';

	return (header.str());
}

void	Response::bodyBuilder()
{
	std::string 		line;

	std::string			path;
	std::stringstream	ss;
	//int					lineCount = 0;

	path = this->_request->getPath();
	std::cout << "Trying to open: " << path << '\n';
	std::ifstream file(path.c_str(), std::ios::binary);
	if (!file)
	{
		std::cerr << "Requested file open error!\n";
		// setCode(404);
		handleERROR(404);
		return;
	}
	std::stringstream buffer;
	buffer << file.rdbuf();
	std::string body = buffer.str();
	replaceAll(body, "{{UPLOAD_BLOCK}}", _request->getUploadDir()["location"]);
	ss << body.size();

	std::cout << "Bytes read: " << ss.str() << std::endl;

	this->_headers["Content-Length"] = ss.str();
	this->_body = body;
}

std::string Response::getMimeType(const std::string &path)
{
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
	if (ext == "cgi") return "text/html";
	if (ext == "py") return "text/html";
	if (ext == "php") return "text/html";
	//we can add more types here
	return "application/octet-stream";
}

bool Response::isUploadsDir(const std::string &path)
{
	if (path.find(_request->getUploadDir()["location"]) != std::string::npos)
		return true;
	//if (path == _request->getUploadDir()["root"]) //COMMENT FOR LATER: double check here if this works / or if it has to be compared to location only
		//return true;
	return (false);
}

bool Response::isCGI(const std::string &path)
{
	if (path.find("/cgi-bin/") != std::string::npos) { return true; }
	if (path.find(".cgi") != std::string::npos) { return true; }		//this would allow to execute scripts which are not in the cgi/bin folder? Maybe we add both conditions in one if?
	return (false);
}

void Response::POSTBodyBuilder()
{
	std::string boundary;
	std::string content_type = _request->getHeader("Content-Type");

	if (content_type.find("multipart/form-data") != std::string::npos)
	{
		size_t boundPos = content_type.find("boundary=");
		if (boundPos == std::string::npos) // no boundary
		{
			handleERROR(400);
			return;
		}
		boundary = content_type.substr(boundPos + 9);
		std::string body = _request->getBody();
		std::vector<std::string> parts = parseMultipartBody(body, boundary);

		std::string filePart;
		bool foundFilePart = false;

		for (size_t i = 0; i < parts.size(); ++i)
		{
			if (parts[i].find("Content-Disposition: form-data;") != std::string::npos &&
				parts[i].find("filename=\"") != std::string::npos)
			{
				filePart = parts[i];
				foundFilePart = true;
				break;
			}
		}
		if (!foundFilePart)
		{
			handleERROR(400);
			return;
		}
		std::string filename = getFilename(filePart);
		std::string fileContent = getFileContent(filePart);

		if (filename.empty())
		{
			std::cout << "No file selected for upload\n";
			handleERROR(400);
			return;
		}
		std::string saveTo = _request->getUploadDir()["root"] + _request->getUploadDir()["location"] + filename;
		std::ofstream outFile(saveTo.c_str(), std::ios::binary);
		if (!outFile)
		{
			handleERROR(500);
			return;
		}
		outFile.write(fileContent.data(), fileContent.size());
		outFile.close();

		this->_headers["Content-Type"] = "text/html";
		std::stringstream ss;
		ss << "<html><body><h1>File uploaded successfully!</h1><p>Saved as: " << filename << "</p></body></html>";
		this->_body = ss.str();
		this->_headers["Content-Length"] = intToString(this->_body.size());
	}
	else 	// if not image, then handle how else
		handleERROR(415);
}
