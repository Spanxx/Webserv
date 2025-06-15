
#include "../incl/Server.hpp"

int	mkdir_p(const std::string fullPath, mode_t mode)
{
	std::istringstream	ss(fullPath);
	std::string			token;
	std::string			currentPath;

	while (getline(ss, token, '/'))
	{
		if (token.empty())
		continue;
		
		if (token == ".." || token == "www")
		{
			currentPath += token + '/';
			continue;
		}

		if (!currentPath.empty() && currentPath[currentPath.size() - 1] != '/')
			currentPath += "/";
		currentPath += token;

		if (mkdir(currentPath.c_str(), mode) != 0)
		{
			if (errno == EEXIST)	// directory already exist
				continue;
			else
			{
				std::cerr << "mkdir failed at: " << currentPath
						  << " (" << std::strerror(errno) << ")\n";
				return (1);
			}
		}
		std::cout << "Directory created: " << currentPath << "\n";
	}
	return (0);
}

void	Server::createDirStructure()
{
	if (this->_name == "")
		this->_name = "defaultServer";

	std::string	serverPath;
	std::string cwd = getcwd(NULL, 0);

	if (cwd != "")
		std::cout << "CWD = " << cwd << std::endl;
	
	if (cwd.find("/src") == std::string::npos)
		serverPath = "www/" + this->_name;
	else
		serverPath = "../www/" + this->_name;

	std::string cgiBin = serverPath + "/cgi-bin";
	std::string error = serverPath + "/error";
	std::string files = serverPath + "/files/uploads";
	std::string html = serverPath + "/html";
	
	if (mkdir_p(cgiBin, 0755) == 1)
		throw ServerException("Creating directory " + cgiBin + " failed!\n");
	if (mkdir_p(error, 0400) == 1)
		throw ServerException("Creating directory " + error + " failed!\n");
	if (mkdir_p(files, 0755) == 1)
		throw ServerException("CCreating directory " + files + " failed!\n");
	if (mkdir_p(html, 0400) == 1)
		throw ServerException("CCreating directory " + html + " failed!\n");
}
