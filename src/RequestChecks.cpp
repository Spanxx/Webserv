#include "../incl/Request.hpp"

int Request::checkURILength()
{
	size_t maxLen = 0;

	//get server config map
	std::map<std::string, std::string> *config = this->_server->getConfigMap("serverConfig");
	if (!config)
	{
		std::cerr << "Config map 'serverConfig' not found!\n";
		this->_code = 500;
		return (1);
	}

	// without line in config --> Default value
	std::map<std::string, std::string>::iterator it = config->find("limitRequestLine");
	if (it == config->end())
	{
		std::cerr << "LimitRequestLine not found in config map!\n";
		std::cerr << "Default value: '4096' used for check!\n";
		maxLen = 4096;
	}
	else	// with line in config
	{
		std::stringstream ss(it->second);
		ss >> maxLen;

		if (ss.fail())
		{
			std::cerr << "Invalid Number in limitRequestLine!\n";
			this->_code = 500;
			return (1);
		}
	}

	if (this->_path.size() > maxLen)
	{
		std::cerr << "Request length is bigger then 'limitRequestLine' in server config!\n";
		this->_code = 414;	// 431? checks all headers??
		return (1);
	}

	return (0);
}

int	Request::checkPathChars()
{
	const std::string allowedChars = "-_./~ ";	//inclued A-Z a-Z 0-9 (checked with isalnum())
	const std::string reservedChars = "!*'();:@&=+$,/?#%";
	const std::string unsafeChars = "`<>\"{}";

	for (int i = 0; this->_path[i]; ++i)
	{
		char	c = this->_path[i];

		//alphanumeric and allowed chars
		if (std::isalnum(c) || allowedChars.find(c) != std::string::npos)
			continue;
			
		//Reserved raw chars - not encoded
		if (reservedChars.find(c) != std::string::npos)
			continue;

		if (unsafeChars.find(c))
			this->_code = 400;

		this->_code = 400;
		return (1);
	}
	return (0);
}

int	Request::checkRequestedPath()
{
	std::cout << "Request path: " << this->_path << '\n';
	//check if path is dir --> send to index.html
	if (!this->_path.empty() && *(this->_path.end() - 1) == '/')
	{
		std::cout << "Client is trying to request a directory --> redirect to www/html/index.html\n";
		this->_path = "/index.html";
		this->_code = 301;
		return (1);
	}

	size_t dotPos = this->_path.find_last_of('.');

	if (dotPos == std::string::npos)
	{
		std::cout << "Requested file has no filetype!\n";
		this->_code = 400;
		return (1);
	}

	std::string ext = this->_path.substr(dotPos + 1);

	std::string newPath;

	//check for cgi, favicon and html page and add directory
	if (this->_path == "/favicon.ico")
		newPath = "www/files" + this->_path;
	else if (ext == "html")
		newPath = "www/html" + this->_path;
	else if (ext == "py" || ext == "js" || ext == "cgi")
		newPath = "www/cgi-bin" + this->_path;
	else if (ext == "png" || ext == "jpg")
		newPath = "www/files" + this->_path;
	else
		newPath = "www" + this->_path;

	this->_path = newPath;

	std::map<std::string, std::string> *dirs = this->_server->getConfigMap("dirConfig");
	if (!dirs)
	{
		std::cerr << "Config map 'dirConfig' not found!\n";
		this->_code = 500;
		return (1);
	}

	const size_t	lastSlash = this->_path.find_last_of("/");
	
	if (lastSlash == std::string::npos)
	{
		std::cerr << "Invalid path: no slash found!\n";
		this->_code = 400;
		return (1);
	}

	std::string dirString = this->_path.substr(0, lastSlash);
	std::cout << "Requested dir: " << dirString << '\n';

	std::map<std::string, std::string>::iterator it = dirs->begin();
	while (it != dirs->end())
	{
		if (dirString == it->second)
		{
			std::cout << "Valid dir found in config file\n";
			return (0);
		}
		++it;
	}

	if (it == dirs->end())
	{
		std::cout << "Invalid dir requested!\n";
		this->_code = 400;
		return (1);
	}

	return (0);
}

int	Request::checkRequestedFiletype()
{	std::map<std::string, std::string> *fileTypes = this->_server->getConfigMap("filetypeConfig");
	if (!fileTypes)
	{
		std::cerr << "Config map 'fileTypesConfig' not found!\n";
		this->_code = 500;
		return (1);
	}

	std::map<std::string, std::string>::iterator it = fileTypes->begin();
	size_t	lastDot = this->_path.find_last_of(".");
	if (lastDot == std::string::npos)
	{
		this->_code = 400;
		return (1);
	}

	std::string typeString = this->_path.substr(lastDot);
	
	std::cout << "Requested filetype: " << typeString << '\n';

	while (it != fileTypes->end())
	{
		if (typeString == it->second)
		{
			std::cout << "Valid filetype found in config file\n";
			return (0);
		}
		else
			++it;
	}
	if (it == fileTypes->end())
	{
		std::cout << "Invalid filetype requested!" << std::endl;
		this->_code = 404;
		return (1);
	}

	return (0);
}
