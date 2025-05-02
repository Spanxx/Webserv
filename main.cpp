
#include "incl/Server.hpp" 
#include "incl/Config.hpp"

void	extractConfigMap(std::ifstream &conFile, std::map<std::string, std::string> &targetMap, std::string target)
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

int	checkConfigFile(std::ifstream &conFile)
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

Config*	createConfig(char *av)
{
	std::ifstream conFile(av);
	if (!conFile)
	{
		std::cout << "Reading config File failed!\n";
		return (NULL);
	}
	if(conFile.peek() == std::ifstream::traits_type::eof())
	{
		std::cout << "The Config file is empty!\n";
		return (NULL);
	}
	
	if (checkConfigFile(conFile) == 1)
		return (NULL);

	std::map<std::string, std::string> serverConfig;
	extractConfigMap(conFile, serverConfig, "server{");
	// std::map<std::string, std::string> dirConfig;
	// extractConfigMap(conFile, serverConfig, "dir{");
	// std::map<std::string, std::string> fileConfig;
	// extractConfigMap(conFile, serverConfig, "files{");

	
	Config *config = new Config(serverConfig);
	// one class for each map or all together?

	return (config);
}

int main(int ac, char **av)
{
	if (ac != 2)
	{
		std::cout << "Please provide a config file [Usage: ./webserv *.conf]\n";
		return (0);
	}

	Config *config = createConfig(av[1]);
	if (!config)
		return (0);

	// browser: 127.0.0.1:555
	Server newServer(config);
	newServer.startListen();
	newServer.serverLoop();

	delete config;
	return (0);
}