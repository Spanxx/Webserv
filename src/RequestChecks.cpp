#include "../incl/Request.hpp"

int	Request::checkPathChars()
{
	const std::string allowedChars = "-_./~";	//inclued A-Z a-Z 0-9 (checked with isalnum())
	const std::string reservedChars = "!*'();:@&=+$,/?#";
	const std::string unsafeChars = "`<>\"{}";

	for (int i = 0; this->_path[i]; ++i)
	{
		char	c = this->_path[i];

		//alphanumeric and allowed chars
		if (std::isalnum(c) || allowedChars.find(c) != std::string::npos)
			continue;
			
		// Handle percent-encoding: %XX (3 characters total)
		if (c == '%' && std::isxdigit(this->_path[i + 1]) && std::isxdigit(this->_path[i + 2]))
		{
			i += 2; // skip over the two hex digits
			continue;
		}

		//Reserved raw chars - not encoded
		if (reservedChars.find(c) != std::string::npos)
			continue;

		return (1);
	}
	return (0);
}

int	Request::checkRequestedPath()
{
	//check if path is dir --> send to mainpage
	if (!this->_path.empty() && *(this->_path.end() - 1) == '/')
	{
		std::cout << "Client is trying to request a directory --> redirect to content/index.html\n";
		this->_path = "content/index.html";
		this->_code = 301;
		return (0);
	}

	std::string newPath;
	//check for favicon and add content
	if (this->_path == "/favicon.ico")
		newPath = "content/files" + this->_path;
	else
		newPath = "content" + this->_path;
	this->_path = newPath;

	std::map<std::string, std::string> *dirs = this->_server->getConfigMap("dirConfig");
	if (!dirs)
	{
		std::cerr << "Config map 'dirConfig' not found!\n";
		return (1);
	}

	const size_t	lastSlash = this->_path.find_last_of("/");
	if (lastSlash == std::string::npos)
	{
		std::cerr << "Invalid path: no slah found!\n";
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
		return (1);
	}

	return (0);
}

int	Request::checkRequestedFiletype()
{	std::map<std::string, std::string> *fileTypes = this->_server->getConfigMap("filetypeConfig");
	if (!fileTypes)
	{
		std::cerr << "Config map 'fileTypesConfig' not found!\n";
		return (1);
	}

	std::map<std::string, std::string>::iterator it = fileTypes->begin();
	int	lastDot = this->_path.find_last_of(".");
	std::string typeString = this->_path.substr(lastDot);
	
	std::cout << "Requested filetype: " << typeString << '\n';

	while (it != fileTypes->end())
	{
		if (typeString == it->second)
		{
			std::cout << "Valid filetype found in config file\n";
			break;
		}
		else
			++it;
	}
	if (it == fileTypes->end())
	{
		std::cout << "Invalid filetype requested!" << std::endl;
		this->_code = 404;
		it = fileTypes->begin();
		return (1);
	}
	//reset map iterator
	it = fileTypes->begin();
	return (0);
}
