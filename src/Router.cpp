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
	// if (dotPos == -1)
	// {
	// 	this->_request->setPath("www/error/status_page.html");
	// 	this->_request->setCode(404);
	// 	throw RouterException("Router exception: Files need an extension!");
	// }

	this->_requestedFile = this->_requestedPath.substr(lastSlashPos + 1);
}

void Router::findDirConfig()
{
	std::map<std::string, std::map<std::string, std::string> >::iterator it = this->_locationBlocks->begin();
	std::map<std::string, std::map<std::string, std::string> >::iterator bestIt = this->_locationBlocks->end();

	size_t longestMatch = 0;
	std::string bestMatch;
	size_t length = 0;

	//std::cout << "Searching Location Block matching requested path = " << this->_requestedPath << std::endl;
	while (it != this->_locationBlocks->end())
	{
		std::string loc_path = it->first;

		if (loc_path.length() > 1)
			loc_path = loc_path.substr(0, loc_path.length() - 1);
		std::cout << "new loc_path for matching " << loc_path << std::endl;
		length = loc_path.length();

		if (this->_requestedPath.compare(0, length, loc_path) == 0)
		{
			if (loc_path.length() > longestMatch)
			{
				longestMatch = loc_path.length();
				bestMatch = it->first;
				bestIt = it;
			}
		}
		++it;
	}
	if (bestIt == this->_locationBlocks->end())
	{
		std::cout << "No locationblock for routing found!\n";
		this->_request->setPath("www/error/status_page.html");
		this->_request->setCode(404);
		throw RouterException("Router exception: No Locationblock for routing found!");
	}

	this->_location = bestMatch;
	this->_dirConfig.clear();
	this->_dirConfig = bestIt->second;

	std::cout << "Location Block ===> " << _location << std::endl;

	this->_locationBlockIndex = this->_dirConfig["index"];
	if (this->_locationBlockIndex.empty())
	{
		std::cout << "No index file defined in location block\n";
		this->_locationBlockIndex = "none";
	}

	this->_locationBlockRoot = this->_dirConfig["root"];
	std::cout << "Locationblock for routing found!\nROOT = " << this->_locationBlockRoot << std::endl;
}

void	Router::checkForDirRequest()
{
	if (_request->getMethod() == "GET" && ((!this->_requestedPath.empty() && *(this->_requestedPath.end() - 1) == '/') || this->_requestedFile.empty()))
	{
		std::cout << "Detected a directory request or missing file.\n";

		if (_dirConfig["autoindex"] == "on")
		{

			std::cout << "Autoindex is on → updating path to autoindex\n";

			//this->_request->setAutoindex(true);
			//this->_request->setPath(this->_serverName + this->_requestedPath + "autoindex.html");
			std::cout << "PATHHHh: " << this->_request->getPath() << "\n";
			this->_requestedFile = "__AUTO_INDEX__";
		}
		else
		{
			if (this->_locationBlockIndex == "none")
			{
				std::cout << "No index file defined in location block, returning 403\n";
				this->_request->setCode(403);
				return ;
			}
			std::cout << "Directory request (autoindex: off) → redirect to corresponding index of the location block "  << "\n";
			this->_requestedFile = this->_locationBlockIndex;
			// this->_request->setPath(this->_locationBlockRoot + this->_requestedPath + this->_requestedFile);
			// this->_mimeType = "text/html";
		}
	}
}

// void	Router::assignFileWithExtension(std::string &type)
// {
// 	std::string	serverRoot = this->_server->getRoot();
// 	std::string	fullPath = checkCwd(serverRoot, false);
	
// 	std::map<std::string, std::string> *config =  this->_server->getConfigMap("typeDirConfig");
// 	std::map<std::string, std::string>::iterator it = config->begin();

// 	while (it != config->end())
// 	{
// 		if (type == it->first)
// 		{
// 			if (it->second == "/files/")
// 			{
// 				std::map<std::string, std::string> uploadDir = this->_server->getUploadDir();
// 				fullPath = uploadDir["root"] + "/" + this->_requestedFile;
// 				std::cout << "Filetype found, redirects to: " << fullPath << '\n';
// 			}
// 			else
// 				fullPath += it->second + this->_requestedFile;
			
// 			std::cout << "Filetype found, redirects to: " << fullPath << '\n';
// 			this->_request->setPath(fullPath);
// 			return ;
// 		}
// 		++it;
// 	}
// 	if (it == config->end())
// 	{
// 		this->_request->setCode(405);
// 		this->_request->setPath("www/error/status_page.html");
// 		throw RouterException("Invalid Filetype requested!");
// 	}
// }

void	Router::setDirForType()
{
	std::string	fullPath;
	std::string type;
	std::string root = this->_server->getRoot();
	fullPath = checkCwd(root, true);

	if (this->_requestedFile == "__AUTO_INDEX__")
	{
		this->_request->setPath(fullPath + this->_requestedPath+ "/__AUTO_INDEX__");
		this->_mimeType = "text/html";
		return;
	}

	if (this->_requestedFile == "" && _request->getMethod() == "GET")
	{
		this->_request->setCode(403);
		this->_mimeType = "text/html";
		return; // needs to return because otherwise the substr(dotPos) was provoking a crash
	}

	size_t 		dotPos = this->_requestedFile.find_last_of(".");

	if (dotPos != std::string::npos && !this->_requestedFile.empty())
	{
		type = this->_requestedFile.substr(dotPos);
	}

	if (_request->getMethod() == "DELETE")
	{
		this->_request->setPath(_dirConfig["root"] + this->_requestedPath); //TODO
		std::cout << "DELETE request, returning full path: " << fullPath << '\n';
		return;
	}

	// if ((type == ".html" || type == ".css") && this->_requestedFile != "status_page.html")
	// 	fullPath += "/html" + this->_requestedPath;
	// if (type == ".py" || type == ".php" || type == ".js")
	// 	fullPath += "/cgi-bin" + this->_requestedPath;
	// if (type == ".png" || type == ".jpg" || type == ".jpeg")
	// 	fullPath += "/files" + this->_requestedPath;



	// std::cout << "FullPath = " << fullPath << '\n';
	//this->_request->setPath(fullPath);
	this->_request->setPath(this->_locationBlockRoot + this->_location + this->_requestedFile);
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


