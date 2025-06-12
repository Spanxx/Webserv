#include	"../incl/Server.hpp"
#include	"../incl/Request.hpp"
#include	"../incl/Response.hpp"

std::map<std::string, std::string>* Server::getConfigMap(const std::string &configName)
{
	if (configName == "serverConfig")
		return(&this->_serverConfig);
	if (configName == "dirConfig")
		return(&this->_dirConfig);
	// if (configName == "pageConfig")
	// 	return(&this->_pageConfig);
	// if (configName == "fileConfig")
	// 	return(&this->_fileConfig);
	// if (configName == "filetypeConfig")
	// 	return(&this->_filetypeConfig);
	else
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
				port = strToInt(item);
				this->_ports.push_back(port);
				++portCounter;
			}
		}
		if (it->first.find("maxbodysize") != std::string::npos)
			_maxBodySize = atoi(it->second.c_str());
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

	std::map<std::string, std::string>::iterator it = config->find("server_name");
	if (it != config->end())
	{
		this->_name = it->second;
		std::cout << "Server Name: " << this->_name << '\n';
	}
	else
	{
		std::cout << "No host in config file, default set to bind to any local address\n";
		this->_name = "default_server"; // default server name
	}
}

void	saveKeyValuePair(std::string &trimmed, std::map<std::string, std::string> &targetMap)
{
	size_t		equalPos = trimmed.find("=");

	if (equalPos != std::string::npos)
	{
		std::string	key = trimmed.substr(0, equalPos - 1);
		std::string value = trimmed.substr(equalPos + 1);
		key = trim(key);
		value = trim(value);
		//std::cout << "config key: " << key << " || value: " << value << '\n';	//debug for config values
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

void	Server::extractConfigMap(std::string &configFile, std::map<std::string, std::string> &targetMap, std::string target)
{
	std::string	line;
	std::string trimmed;
	bool		inBlock = false;

	std::istringstream iss(configFile);
	if (!iss)
		throw ServerException("Extracting config file failed!\n");

	while (getline(iss, line))
	{
		trimmed = line;
		trimmed.erase(0, trimmed.find_first_not_of(" \t"));
		if (trimmed.find(target) != std::string::npos)
		{
			//save dir before continue in dirblock
			if (target == "location")
			{
				if (trimmed.find_first_of('{') != std::string::npos)
					trimmed.erase(trimmed.find_first_of('{'));	//remove curly bracket at end of location
				saveKeyValuePair(trimmed, targetMap);		//save location
			}

			while (getline(iss, line))
			{
				trimmed = line;
				trimmed.erase(0, trimmed.find_first_not_of(" \t"));
				// trimmed.erase(trimmed.find_last_not_of(" \t") + 1);

				if (trimmed.empty() || trimmed[0] == '#')
					continue;

				if (trimmed == "}" && !inBlock)
				{
					iss.clear();                // Clear any error flags
					iss.seekg(0, std::ios::beg); // Go back to the beginning
					return ;
				}

				//skip location block if extracting server config
				if (!inBlock && trimmed.find("location") == 0 && trimmed.find("{") != std::string::npos)
				{
					inBlock = true;
					continue; // skip this line too
				}

				// Check for end of block
				if (inBlock && trimmed.find("}") == std::string::npos)
					continue; // skip closing brace line
				else if (inBlock && trimmed.find("}") != std::string::npos)
				{
					inBlock = false;
					continue;
				}
				saveKeyValuePair(trimmed, targetMap);
			}
		}
	}
}

void	Server::loadMimeTypes()
{
	std::string line;
	std::string mimeConfig;

	std::ifstream file("www/config/mime.types");
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

// int	Server::createConfig(char *av)
int	Server::createConfig(char *av, std::string &serverConfig)
{
	std::string filePath = checkFilePath(av);

	this->extractConfigMap(serverConfig, _serverConfig, "server");
	this->extractConfigMap(serverConfig, _dirConfig, "location");
	this->loadMimeTypes();
	return (0);
}
