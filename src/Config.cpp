
#include "../incl/Config.hpp"

Config::Config(std::map<std::string, std::string> serverConfig)
{
	try
	{
		for (std::map<std::string, std::string>::iterator it = serverConfig.begin(); it != serverConfig.end(); ++it)
		{	
			if (it->first == "host)")
			this->_host = it->second;
			else if (it->first == "root")
			this->_root = it->second;
			else if (it->first == "port")
				this->_port = std::atoi(it->second.c_str());
			else if (it->first == "index")
				this->_index = it->second;
			else
				ConfigException("Invalid key found in config file!");
		}
	}
	catch (std::exception &e)
	{
		std::cout << e.what() << std::endl;
		return ;
	}
	std::cout << "Config constructed\n";
}

Config::Config(Config &other)
{
	this->_port = other._port;
	this->_host = other._host;
	this->_root = other._root;
	this->_index = other._index;
	// this->_tryFiles = other._tryFiles;

	std::cout << "Config copied\n";
}

Config::~Config()
{
	std::cout << "Config deconstructed\n";
}

Config& Config::operator=(Config &other)
{
	if (this == &other)
		return (*this);

	this->_port = other._port;
	this->_host = other._host;
	this->_root = other._root;
	this->_index = other._index;
	// this->_tryFiles = other._tryFiles;

	std::cout << "Config assigned\n";

	return (*this);
}

Config::ConfigException::ConfigException(std::string error) : std::runtime_error(error) {}

int Config::getPort()
{
	return (this->_port);
}

void	Config::extractConfigMap(std::ifstream &conFile, std::map<std::string, std::string> &targetMap, std::string target)
{
	std::string	line;

	while (std::getline(conFile, line))
	{
		if (line == target)
		{
			while (std::getline(conFile, line))
			{
				if (line == "}")
					break;

				if (line.empty() || line[0] == '#')
					continue;
				
				size_t	equalPos = line.find('=');
				if (equalPos != std::string::npos)
				{
					std::string	key = line.substr(0, equalPos);
					std::string value = line.substr(equalPos + 1);

					//check for key duplicates
					if (targetMap.find(key) != targetMap.end())
					{
						std::cout << "Warning: Duplicate key found " << key << '\n'	// check how nginx handles it
								<< "new Value don't overwrites existing Value!\n";
					}
					else
						targetMap[key] = value;
				}
			}
		}
	}
}

int	Config::checkConfigFile(std::ifstream &conFile)
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
				std::cout << "Error: Forbidden char <space> found in config file!\n";
				return (1);
			}
		}
		// maybe add more checks?
	}
	conFile.clear();                // Clear any error flags
	conFile.seekg(0, std::ios::beg); // Go back to the beginning
	return (0);
}