#include "../incl/Router.hpp"
#include "../incl/Server.hpp"
#include "../incl/Libraries.hpp"
#include "../incl/Utils.hpp"


Router::Router(Server *server, Request *request) : _server(server), _request(request)
{
	std::cout << "Router created\n";

	this->_requestedPath = this->_request->getPath();
	this->_serverName = this->_server->getName();
	this->_locationBlocks = this->_server->getLocationBlocks();

	try
	{
		extractPath();
		extractFile();
		findDirConfig();
		checkForDirRequest();
		setDirForType();
		handleFavicon();
		checkMethods();
	}
	catch (std::exception &e)
	{
		std::cerr << "Router exception: " << e.what() << std::endl;
	}
}

Router::Router(Router &other)
{
	this->_server = other._server;
	this->_request = other._request;
	this->_requestedPath = other._requestedPath;
	this->_requestedFile = other._requestedFile;
	this->_serverName = other._serverName;
	this->_locationBlocks = other._locationBlocks;
	this->_dirConfig = other._dirConfig;
	this->_extractedPath = other._extractedPath;

	std::cout << "Router copied\n";
}

Router& Router::operator=(Router &other)
{
	if (this == &other)
		return (*this);

	this->_server = other._server;
	this->_request = other._request;
	this->_requestedPath = other._requestedPath;
	this->_requestedFile = other._requestedFile;
	this->_serverName = other._serverName;
	this->_locationBlocks = other._locationBlocks;
	this->_dirConfig = other._dirConfig;
	this->_extractedPath = other._extractedPath;

	std::cout << "Router assigned\n";

	return (*this);
}

Router::~Router()
{
	std::cout << "Router deconstructed\n";
}

Router::RouterException::RouterException(const std::string &error) : std::runtime_error(error) {}

void	Router::extractPath()
{
	std::cout << "Requested Path: " << this->_requestedPath << '\n';
	// "/index.html"

	int 		lastSlashPos = -1;

	lastSlashPos = this->_requestedPath.find_last_of('/');
	if (lastSlashPos == -1)
	{
		std::cout << "Invalid Request (no slash)!\n";
		this->_request->setCode(404);
		return ;
	}

	this->_extractedPath = this->_requestedPath.substr(0, lastSlashPos + 1);
}

void	Router::extractFile()
{
	int 		dotPos 		= -1;
	int 		lastSlashPos = -1;
	std::map<std::string, std::string> uploadLocationBlock = this->_server->getUploadDir();

	std::string uploadDir = uploadLocationBlock["location"];
	int start = uploadDir.find_first_of("/");
	int end = uploadDir.find_last_of("/");

	uploadDir = uploadDir.substr(start + 1, end - 1);

	dotPos = this->_requestedPath.find_last_of('.');
	lastSlashPos = this->_requestedPath.find_last_of('/');

	//check if directory was set to requested file
	if (dotPos == -1)
	{
		this->_request->setPath("www/error/status_page.html");
		this->_request->setCode(404);
		throw RouterException("Router exception: Files need an extension!");
	}

	this->_requestedFile = this->_requestedPath.substr(lastSlashPos + 1);
}

void	Router::findDirConfig()
{
	std::map<std::string, std::map<std::string, std::string> >::iterator it = this->_locationBlocks->begin();

	while (it != this->_locationBlocks->end())
	{
		if (this->_extractedPath == it->first)
		{
			_dirConfig.clear();
			_dirConfig = it->second;
			std::cout << "Locationblock for routing found!\n";
			return ;
		}
		++it;
	}

	if (it == this->_locationBlocks->end())
	{
		std::cout << "No locationblock for routing found!\n";
		this->_request->setPath("www/error/status_page.html");
		this->_request->setCode(404);
		throw RouterException("Router exception: No Locationblock for routing found!");
	}
}

void	Router::checkForDirRequest()
{
	if (_request->getMethod() == "GET" && ((!this->_requestedPath.empty() && *(this->_requestedPath.end() - 1) == '/') || this->_requestedFile.empty()))
	{
		std::cout << "Detected a directory request or missing file.\n";

		if (_dirConfig["autoindex"] == "on")
		{
			std::cout << "Autoindex for directory " << _requestedPath << " is on → setting CGI path for directory listing\n";
			this->_request->setPath(this->_server->getRoot() + "/cgi-bin/autoindex.py");	// path to script for listing files in folder
			this->_requestedFile = "autoindex.py";
			if (this->_requestedPath == "/") // TODO Confirm if is the best way to do it?
				this->_requestedPath = "/autoindex.py";
		}
		else
		{
			std::string indexFile = "index.html"; // make it dynamic?
			std::string redirectPath = this->_server->getRoot() + "/html/" + indexFile;

			std::cout << "Directory request (autoindex: off) → redirect to " << redirectPath << "\n";
			this->_request->setPath(redirectPath);	//change it to index from serverblock (for redirect Path)
			this->_requestedPath = "/" + indexFile;
			this->_requestedFile = indexFile;
		}
	}
}

void	Router::assignFileWithExtension(std::string &type)
{
	std::string	serverRoot = this->_server->getRoot();
	std::string	fullPath = checkCwd(serverRoot, false);
	
	std::map<std::string, std::string> *config =  this->_server->getConfigMap("typeDirConfig");
	std::map<std::string, std::string>::iterator it = config->begin();

	while (it != config->end())
	{
		if (type == it->first)
		{
			if (it->second == "/files/")
			{
				std::map<std::string, std::string> uploadDir = this->_server->getUploadDir();
				fullPath = uploadDir["root"] + "/" + this->_requestedFile;
				std::cout << "Filetype found, redirects to: " << fullPath << '\n';
			}
			else
				fullPath += it->second + this->_requestedFile;
			
			std::cout << "Filetype found, redirects to: " << fullPath << '\n';
			this->_request->setPath(fullPath);
			return ;
		}
		++it;
	}
	if (it == config->end())
	{
		this->_request->setCode(405);
		this->_request->setPath("www/error/status_page.html");
		throw RouterException("Invalid Filetype requested!");
	}
}

void	Router::setDirForType()
{
	// if (this->_requestedFile == "" && _request->getMethod() == "GET")
	// {
	// 	this->_request->setCode(403); // TODO
	// 	std::cout << "_requestedFile = EMPTY --> returning" << std::endl;
	// 	this->_mimeType = "text/html";
	// 	return; // needs to return because otherwise the substr(dotPos) was provoking a crash
	// } //KEEP

	std::string	type;
	std::string	fullPath;
	size_t 		dotPos = this->_requestedFile.find_last_of(".");

	if (dotPos != std::string::npos && !this->_requestedFile.empty())
	{
		type = this->_requestedFile.substr(dotPos);
		assignFileWithExtension(type);
	}
}

void	Router::handleFavicon()
{
	std::string	serverRoot = this->_server->getRoot();
	std::string newPath = checkCwd(serverRoot, false);

	if (this->_requestedFile == "favicon.ico")
	{
		newPath += "/files/favicon.ico";
		this->_request->setPath(newPath);
	}
}

void	Router::checkMethods()
{
	std::string	method = this->_request->getMethod();
	std::map<std::string, std::string>::iterator it = this->_dirConfig.begin();

	while (it != this->_dirConfig.end())
	{
		if (it->first == "methods")
		{
			if (it->second.find(method) != std::string::npos)
				std::cout << "Request Method is allowed: "<< method << '\n'; 
			return ;
		}
		++it;
	}
	// Method not found
	this->_request->setCode(405);
	throw RouterException("Method is not allowed!");
}

// void	Router::checkMethods()
// {
// 	std::map<std::string, std::map<std::string, std::string> >::iterator it = this->_locationBlocks->begin();

// 	while (it != this->_locationBlocks->end())
// 	{
// 		if (this->_extractedPath == it->first)
// 		{
// 			std::map<std::string, std::string>::iterator it_conf = it->second.find("methods");
// 			if (it_conf != it->second.end())
// 			{
// 				if (it_conf->second.find(_request->getMethod()) == std::string::npos)
// 					this->_request->setCode(405);
// 				return ;
// 			}
// 		}
// 		++it;
// 	}
// }


