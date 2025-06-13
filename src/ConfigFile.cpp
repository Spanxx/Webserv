#include	"../incl/Server.hpp"
#include	"../incl/Request.hpp"
#include	"../incl/Response.hpp"
#include	"../incl/Utils.hpp"

void	createConfigList(std::string configPath, std::vector<std::string> &configList)
{
	bool			inServerBlock = false;
	bool			inLocationBlock = false;

	std::string		line;
	std::string		serverConfig;
	std::ifstream	iss(configPath.c_str());
	if (!iss)
	{
		std::cerr << "Creating config list failed!\n";
		return;
	}

	while (getline(iss, line))
	{
		if (line.find("#") != std::string::npos || line.empty())
			continue;
		if (!inServerBlock && line.find("server") != std::string::npos)
			inServerBlock = true;
		else if (inServerBlock && line.find("{") != std::string::npos)
			inLocationBlock = true;
		else if (inServerBlock && inLocationBlock && line.find("}") != std::string::npos)
			inLocationBlock = false;
		else if (inServerBlock && !inLocationBlock && line.find("}") != std::string::npos)
		{
			serverConfig.append(line);
			inServerBlock = false;
			configList.push_back(serverConfig);
			serverConfig.clear();
			continue;
		}	
		line += '\n';
		serverConfig.append(line);
	}
}

std::map<std::string, std::string>* Server::getConfigMap(const std::string &configName)
{
	if (configName == "serverConfig")
		return(&this->_serverConfig);
	if (configName == "dirConfig")
		return(&this->_dirConfig);
	if (configName == "mimeConfig")
		return(&this->_mimetypeConfig);
	
	return (NULL);
}

int	Server::checkConfigFile(std::ifstream &conFile)
{
	std::string	line;
	while(std::getline(conFile, line))
	{
		for(int i = 0; line[i] != '\n' && line[i]; ++i)
		{
			if (line[0] == '#')
				break ;
			if (line[i] == 32)	//space
			{
				std::cerr << "Error: Forbidden char <space> found in config file!\n";
				return (1);
			}
			if (line[i] == '=' && line[i + 1] == '-')	//negative value found //  must be changed for new version 111,222,333
			{
				std::cerr << "Error: Non-positive value found in config file!\n";
				return (1);
			}
		}
		// maybe add more checks?
	}
	conFile.clear();                // Clear any error flags
	conFile.seekg(0, std::ios::beg); // Go back to the beginning
	return (0);
}

void	Server::extractPorts()
{
	int			port = 0;
	int			portCounter = 0;
	std::string item;

	std::map<std::string, std::string> *config = getConfigMap("serverConfig");

	if (!config)
		throw ServerException("Extracting serverConfig map failed!");

	std::map<std::string, std::string>::iterator it = config->begin();
	while (it != config->end())
	{
		if (it->first.find("listen") != std::string::npos)
		{
			std::stringstream ss(it->second);

			while (std::getline(ss, item, ','))
			{
				if (!safeAtoi(item, port) || port < 1024 || port > 65535) //below 1024 only with sudo rights
					throw ServerException("Ports need to be between 1024 and 65535");
				this->_ports.push_back(port);
				++portCounter;
			}
		}
		if (it->first.find("maxbodysize") != std::string::npos)
		{
			int size;
			if (!safeAtoi(it->second, size) || size < 0 || size > 10485760)
				throw ServerException("Max body size needs to be between 0 and 10MB");
			_maxBodySize = static_cast<size_t>(size);
		}
		++it;
	}

	this->_numPorts = portCounter;

	std::vector<int>::iterator iPorts = this->_ports.begin();
	while (iPorts != this->_ports.end())
	{
		std::cout << "Port: " << *iPorts << '\n';	// add  check for port duplicates
		++iPorts;
	}
}

void	Server::extractHost()
{
	std::map<std::string, std::string> *config = getConfigMap("serverConfig");

	if (!config)
		throw ServerException("Extracting serverConfig map failed!");

	std::map<std::string, std::string>::iterator it = config->find("host");
	if (it != config->end())
	{
		if (!isValidIP(it->second))
			throw ServerException("Host IP needs to be within a private or loopback range");
		this->_IPHost = it->second;
		std::cout << "Host: " << this->_IPHost << '\n';
	}
	else
	{
		std::cout << "No host in config file, default set to bind to any local address\n";
		this->_IPHost = "0.0.0.0"; // bind to any local address
	}
}

void	Server::extractName()
{
	std::map<std::string, std::string> *config = getConfigMap("serverConfig");

	if (!config)
		throw ServerException("Extracting serverConfig map failed!");

	std::map<std::string, std::string>::iterator it = config->find("name");
	if (it != config->end())
	{
		this->_name = it->second;
		std::cout << "Server Name: " << this->_name << '\n';
	}
	else
	{
		std::cout << "No name in config file, default set to defaultServer!\n";
		this->_name = "default_server"; // default server name
	}
}

void	saveKeyValuePair(std::string &trimmed, std::map<std::string, std::string> &targetMap, std::string *host, std::string *locationPath)
{
	size_t		equalPos = trimmed.find("=");

	if (equalPos != std::string::npos)
	{
		std::string	key = trimmed.substr(0, equalPos - 1);
		std::string value = trimmed.substr(equalPos + 1);
		key = trim(key);
		value = trim(value);

		if (key == "host" && value != "")
			*host = value;
		if (key == "location" && value != "")
			*locationPath = value;

		// std::cout << "config key: " << key << " || value: " << value << '\n';	//debug for config values
		//check for key duplicates
		if (targetMap.find(key) != targetMap.end())
		{
			//std::cout << "Warning: Duplicate key found " << key << '\n'	// check how nginx handles it
					//<< "new Value don't overwrites existing Value!\n";
		}
		else
			targetMap[key] = value;
	}
}

int	handleLocationBlocks(bool *inBlock, std::string &trimmed)
{
	if (!(*inBlock) && trimmed.find("location") == 0 && trimmed.find("{") != std::string::npos)
	{
		*inBlock = true;
		return (1);
	}
	// Check for end of block
	if (*inBlock && trimmed.find("}") == std::string::npos)
		return (1);
	
	if (*inBlock && trimmed.find("}") != std::string::npos)
	{
		*inBlock = false;
		return (1);
	}

	return (0);
}


void	Server::extractConfigMap(std::string &configFile, std::map<std::string, std::string> &targetMap, std::string target)
{
	std::string	line;
	std::string trimmed;
	std::string locationPath;
	bool		inBlock = false;

	std::istringstream iss(configFile);
	if (!iss)
		throw ServerException("Extracting config file failed!\n");

	while (getline(iss, line))
	{
		trimmed = trim(line);
		if (trimmed.find(target) != std::string::npos)
		{
			//save dir before continue in dirblock
			if (target == "location")
			{
				if (trimmed.find_first_of('{') != std::string::npos)
					trimmed.erase(trimmed.find_first_of('{'));	//remove curly bracket at end of location
				saveKeyValuePair(trimmed, targetMap, &this->_IPHost, &locationPath);		//save location
			}

			while (getline(iss, line))
			{
				trimmed = trim(line);

				if (trimmed.empty() || trimmed[0] == '#')
					continue;

				if (trimmed == "}" && !inBlock)
				{
					if (target == "location" && !targetMap.empty())
					{
						this->_locationBlocks[locationPath] = targetMap;
						targetMap.clear();
						break;
					}
				}
				
				if (handleLocationBlocks(&inBlock, trimmed) == 1)
					continue;

				saveKeyValuePair(trimmed, targetMap, &this->_IPHost, &locationPath);
			}

		}
	}
}

void	Server::loadMimeTypes()
{
	std::string line;
	std::string mimeConfig;
	std::string cwd = getcwd(NULL, 0);
	std::string fullPath;

	if (cwd.find("/src") == std::string::npos)
		fullPath = "www/config/mime.types";
	else
		fullPath = "../www/config/mime.types";

	std::ifstream file(fullPath.c_str());
	if (!file)
		throw ServerException("Loading mime.types failed!");

	while(getline(file, line))
	{
		if (line.empty() || line.find('#') != std::string::npos)
			continue;
		line += '\n';
		mimeConfig.append(line);

	}
	this->extractConfigMap(mimeConfig, _mimetypeConfig, "types");
}

// int	Server::createConfig(char *av, std::string &serverConfig)
int	Server::createConfig(std::string &serverConfig)
{
	// std::string filePath = checkFilePath(av);

	std::map<std::string, std::string>	_dirConfig;

	this->extractConfigMap(serverConfig, _serverConfig, "server");
	this->extractConfigMap(serverConfig, _dirConfig, "location");
	this->loadMimeTypes();
	return (0);
}

void Server::assignUploadDir()
{
	for (std::map<std::string, std::map<std::string, std::string> >::iterator it = _locationBlocks.begin(); it != _locationBlocks.end(); ++it)
	{
		std::string location = it->first;

		if (location == "/" || location == "/cgi-bin/" || location == "/files/" ||location.find("html") != std::string::npos)
			continue;
		bool allowPOST = checkPOST(it->second); //COMMENT FOR LATER: should we add more checks if the config block is ok?
		if (allowPOST)
		{
			if (_uploadDir.empty() || location.find("upload") != std::string::npos)
			{
				_uploadDir = location;
				std::cout << "Assigned upload dir: " << _uploadDir << std::endl;
				if (location.find("upload") != std::string::npos)
					return;
			}
		}
	}
	throw ServerException("Config file needs to assign uploads directory (required to allow POST method), cannot be root, files, cgi-bin or name containing html");
}

bool Server::checkPOST(std::map<std::string, std::string> configblock)
{
	std::map<std::string, std::string>::iterator it = configblock.find("methods");
	if (it != configblock.end() && it->second.find("POST") != std::string::npos)
		return true;
	return false;
}
