#include	"../incl/Server.hpp"
#include	"../incl/Request.hpp"
#include	"../incl/Response.hpp"

std::map<std::string, std::string>* Server::getConfigMap(const std::string &configName)
{
	if (configName == "serverConfig")
		return(&this->_serverConfig);
	if (configName == "dirConfig")
		return(&this->_dirConfig);
	if (configName == "pageConfig")
		return(&this->_pageConfig);
	if (configName == "fileConfig")
		return(&this->_fileConfig);
	if (configName == "filetypeConfig")
		return(&this->_filetypeConfig);
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
		if (it->first.find("ports") != std::string::npos)
		{	
			std::stringstream ss(it->second);

			while (std::getline(ss, item, ','))
			{
				port = strToInt(item);
				this->_ports.push_back(port);
				++portCounter;
			}
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

void	Server::extractConfigMap(std::ifstream &conFile, std::map<std::string, std::string> &targetMap, std::string target)
{
	std::string	line;

	while (std::getline(conFile, line))
	{
		if (line == target)
		{
			while (std::getline(conFile, line))
			{
				if (line == "}")
				{
					conFile.clear();                // Clear any error flags
					conFile.seekg(0, std::ios::beg); // Go back to the beginning	
					return ;
				}

				if (line.empty() || line[0] == '#')
					continue;
				
				size_t	equalPos = line.find('=');
				if (equalPos != std::string::npos)
				{
					std::string	key = line.substr(0, equalPos);
					std::string value = line.substr(equalPos + 1);
					// std::cout << "config key: " << key << " || value: " << value << '\n';	debug for config values
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

int	Server::createConfig(char *av)
{
	std::string filePath = checkFilePath(av);

	// std::ifstream configFile(av);
	std::ifstream configFile(filePath.c_str());
	if (!configFile)
	{
		std::cout << "Reading config File failed!\n";
		return (1);
	}
	if (configFile.peek() == std::ifstream::traits_type::eof())
	{
		std::cout << "The Config file is empty!\n";
		return (1);
	}

	if (Server::checkConfigFile(configFile) == 1)
		return (1);

	this->extractConfigMap(configFile, _serverConfig, "server{");
	this->extractConfigMap(configFile, _dirConfig, "dir{");
	this->extractConfigMap(configFile, _pageConfig, "pages{");
	this->extractConfigMap(configFile, _fileConfig, "files{");
	this->extractConfigMap(configFile, _filetypeConfig, "filetypes{");

	return (0);
}
