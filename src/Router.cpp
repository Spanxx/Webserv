#include "../incl/Router.hpp"
#include "../incl/Server.hpp"


Router::Router(Server *server, Request *request) : _server(server), _request(request)
{
	std::cout << "Router created\n";

	this->_requestedPath = this->_request->getPath();
	this->_serverName = this->_server->getName();
	this->_locationBlocks = this->_server->getLocationBlocks();

	extractPathAndFile();
	checkMethods();
	findDirConfig();
	// checkDirPermission();
	checkForDirRequest();
	setDirForType();
	handleFavicon();
	// handleAutoIndex();
	// handleRedir();
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

std::string	Router::checkCwd()
{
	std::string cwd;
	std::string path;

	char* rawCwd = getcwd(NULL, 0);
	if (rawCwd)
	{
		cwd = rawCwd;
		free(rawCwd);
	}
	else{}
		//TODO
	if (cwd.find("/src") != std::string::npos)
		path = "../www/" + this->_serverName;
	else
		path = "www/" + this->_serverName;

	return (path);
}

void	Router::extractPathAndFile()
{
	std::cout << "Requested Path: " << this->_requestedPath << '\n';

	int 		dotPos 		= -1;
	int 		lastSlashPos = -1;
	std::string	newPath;

	dotPos = this->_requestedPath.find_last_of('.');
	lastSlashPos = this->_requestedPath.find_last_of('/');

	if (lastSlashPos == -1)
	{
		std::cout << "Invalid Request (no slash), code 404\n";
		this->_request->setCode(404);
		return;
	}

	this->_extractedPath = this->_requestedPath.substr(0, lastSlashPos + 1);

	if (dotPos != -1)
	{
		this->_requestedFile = this->_requestedPath.substr(lastSlashPos + 1);
	}
}

void Router::findDirConfig()
{
	std::map<std::string, std::map<std::string, std::string> >::iterator it = this->_locationBlocks->begin();
	std::map<std::string, std::map<std::string, std::string> >::iterator bestIt = this->_locationBlocks->end();

	size_t longestMatch = 0;
	std::string bestMatch;

	std::cout << "Searching Location Block matching requested path = " << this->_requestedPath << std::endl;
	while (it != this->_locationBlocks->end())
	{
		const std::string& loc_path = it->first;
		if (this->_requestedPath.compare(0, loc_path.length(), loc_path) == 0)
		{
			if (loc_path.length() > longestMatch)
			{
				longestMatch = loc_path.length();
				bestMatch = loc_path;
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
		return;
	}

	this->_location = bestMatch;
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

void	Router::checkDirPermission()
{

}

void	Router::setDirForType()
{
	std::string	fullPath;
	fullPath = checkCwd();

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
	std::string	type = "";

	if (dotPos != std::string::npos) {
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
	this->_request->setPath(this->_locationBlockRoot + this->_location + "/"+ this->_requestedFile);
}

void	Router::handleFavicon()
{
	std::string newPath = checkCwd();

	if (this->_requestedFile == "favicon.ico")
	{
		newPath += "/files/favicon.ico";
		this->_request->setPath(newPath);
	}
}

void	Router::handleRedir()
{

}

void	Router::handleAutoIndex()
{

}

void	Router::checkMethods()
{
	std::map<std::string, std::map<std::string, std::string> >::iterator it = this->_locationBlocks->begin();

	while (it != this->_locationBlocks->end())
	{
		if (this->_extractedPath == it->first)
		{
			std::map<std::string, std::string>::iterator it_conf = it->second.find("methods");
			if (it_conf != it->second.end())
			{
				if (it_conf->second.find(_request->getMethod()) == std::string::npos)
					this->_request->setCode(405);
				return ;
			}
		}
		++it;
	}
}


