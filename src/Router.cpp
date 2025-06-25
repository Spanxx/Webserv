#include "../incl/Router.hpp"
#include "../incl/Server.hpp"


Router::Router(Server *server, Request *request) : _server(server), _request(request)
{
	std::cout << "Router created\n";

	this->_requestedPath = this->_request->getPath();
	this->_serverName = this->_server->getName();
	this->_locationBlocks = this->_server->getLocationBlocks();

	extractPathAndFile();
	findDirConfig();
	// checkDirPermission();
	checkForDirRequest();
	setDirForType();
	handleFavicon();
	// handleAutoIndex();
	// handleRedir();
	checkMethods();
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
	// "/index.html"

	int 		dotPos 		= -1;
	int 		lastSlashPos = -1;
	std::string	newPath;

	dotPos = this->_requestedPath.find_last_of('.');
	lastSlashPos = this->_requestedPath.find_last_of('/');

	if (lastSlashPos == -1)
	{
		std::cout << "Invalid Request (no slash), redirect to index.html\n";

		this->_request->setCode(404);

		dotPos = this->_requestedPath.find_last_of('.');
		lastSlashPos = this->_requestedPath.find_last_of('/');
	}

	this->_extractedPath = this->_requestedPath.substr(0, lastSlashPos + 1);

	if (dotPos != -1)
	{
		this->_requestedFile = this->_requestedPath.substr(lastSlashPos + 1);
	}
}

void	Router::findDirConfig()
{
	std::map<std::string, std::map<std::string, std::string> >::iterator it = this->_locationBlocks->begin();

	while (it != this->_locationBlocks->end())
	{
		if (this->_extractedPath == it->first)
		{
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
	}
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
			this->_request->setPath(this->_serverName + this->_requestedPath + "autoindex.html");
			std::cout << "PATHHHh: " << this->_request->getPath() << "\n";
			this->_requestedFile = "autoindex.html"
			// this->_request->setPath("www/" + _serverName + "/cgi-bin/autoindex.py");	// path to script for listing files in folder
			// this->_requestedFile = "autoindex.py";
			// if (this->_requestedPath == "/") // TODO Confirm if is the best way to do it?
			// 	this->_requestedPath = "/autoindex.py";
		}
		else
		{
			std::string indexFile = "index.html"; // make it dynamic?

			std::cout << "Directory request (autoindex: off) → redirect to index.html "  << "\n";
			this->_requestedFile = indexFile; // set the requested file to index.html
			this->_requestedPath = "/index.html"; //TODO: dynamic
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

	if (this->_requestedFile == "autoindex.html")
	{
		this->_request->setPath(fullPath + this->_requestedPath+ "/autoindex.html");
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

	if ((type == ".html" || type == ".css") && this->_requestedFile != "status_page.html")
		fullPath += "/html" + this->_requestedPath;
	if (type == ".py" || type == ".php" || type == ".js")
		fullPath += "/cgi-bin" + this->_requestedPath;
	if (type == ".png" || type == ".jpg" || type == ".jpeg")
		fullPath += "/files" + this->_requestedPath;



	std::cout << "FullPath = " << fullPath << '\n';
	this->_request->setPath(fullPath);
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


