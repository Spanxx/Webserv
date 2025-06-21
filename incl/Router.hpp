#ifndef ROUTER_HPP
#define ROUTER_HPP

#include "Server.hpp"
#include "Request.hpp"
#include "Response.hpp"

class Router
{
public:
	Router(Server *server, Request *request);
	Router(Router &other);
	Router& operator=(Router &other);
	~Router();

	void		extractPathAndFile();
	void		findDirConfig();
	void		checkForDirRequest();
	void		handleFavicon();
	void		setDirForType();
	void		checkDirPermission();
	void		handleRedir();
	void		handleAutoIndex();
	void		checkMethods();
	std::string	checkCwd();


private:
	Server 														*_server;
	Request 													*_request;
	std::string													_serverName;
	std::string													_requestedPath;
	std::string													_requestedFile;
	std::string													_extractedPath;
	std::string													_mimeType;
	std::map<std::string, std::string>							_dirConfig;
	std::map<std::string, std::map<std::string, std::string> >	*_locationBlocks;
};


#endif	//ROUTER_HPP

// Location structs
	// std::map<std::string, std::string>													_dirConfig;
	// std::map<std::string, std::map<std::string, std::string> >							_locationBlocks;

// int	Request::checkRequestedPath()
// {
// 	std::cout << "Request path: " << this->_path << '\n';
// 	//check if path is dir --> send to index.html
// 	if (!this->_path.empty() && *(this->_path.end() - 1) == '/')
// 	{
// 		std::cout << "Client is trying to request a directory --> redirect to www/html/index.html\n";
// 		this->_path = "www/html/index.html";
// 		this->_code = 200;
// 		return (1);
// 	}

// 	size_t dotPos = this->_path.find_last_of('.');

// 	if (dotPos == std::string::npos)
// 	{
// 		std::cout << "Requested file has no filetype!\n";
// 		this->_code = 400;
// 		return (1);
// 	}

// 	std::string ext = this->_path.substr(dotPos + 1);

// 	std::string newPath;

// 	//check for cgi, favicon and html page and add directory
// 	if (this->_path == "/favicon.ico")
// 		newPath = "www/files" + this->_path;
// 	else if (ext == "html")
// 		newPath = "www/html" + this->_path;
// 	else if (ext == "py" || ext == "js" || ext == "cgi")
// 		newPath = "www/cgi-bin" + this->_path;
// 	else if (ext == "png" || ext == "jpg" || ext == "jpeg" || ext == "gif" || ext == "svg" || ext =="pdf" || ext == "HEIC")
// 		newPath = "www/files" + this->_path;
// 	else
// 		newPath = "www" + this->_path;

// 	this->_path = newPath;

// 	std::map<std::string, std::string> *dirs = this->_server->getConfigMap("dirConfig");
// 	if (!dirs)
// 	{
// 		std::cerr << "Config map 'dirConfig' not found!\n";
// 		this->_code = 500;
// 		return (1);
// 	}

// 	const size_t	lastSlash = this->_path.find_last_of("/");

// 	if (lastSlash == std::string::npos)
// 	{
// 		std::cerr << "Invalid path: no slash found!\n";
// 		this->_code = 400;
// 		return (1);
// 	}

// 	std::string dirString = this->_path.substr(0, lastSlash);
// 	std::cout << "Requested dir: " << dirString << '\n';

// 	std::map<std::string, std::string>::iterator it = dirs->begin();
// 	while (it != dirs->end())
// 	{
// 		if (dirString == it->second)
// 		{
// 			std::cout << "Valid dir found in config file\n";
// 			return (0);
// 		}
// 		++it;
// 	}

// 	if (it == dirs->end())
// 	{
// 		std::cout << "Invalid dir requested!\n";
// 		this->_code = 400;
// 		return (1);
// 	}

// 	return (0);
// }
