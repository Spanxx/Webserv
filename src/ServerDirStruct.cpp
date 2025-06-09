
#include "../incl/Server.hpp"

void	Server::createDirStructure()
{
	
}
void	Server::mkdir_p(const std::string fullPath, mode_t mode)
{
	std::istringstream	ss(fullPath);
	std::string			token;
	std::string			currentPath;

	while (getline(ss, token, '/'))
	{
		if (token.empty())
			continue;

		currentPath += "/" + token;
		if (mkdir(currentPath.c_str(), mode) != 0)
		{
			if (errno == EEXIST)	// directory already exist
				continue;
			else
				throw ServerException("Creating directory structure failed!");
		}
	}
}
