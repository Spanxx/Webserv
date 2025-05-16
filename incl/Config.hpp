
#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "Server.hpp"

class Config
{
public:
	Config(std::map<std::string, std::string> serverConfig);
	Config(Config &other);
	~Config();

	Config& operator=(Config &other);

	class ConfigException : public std::runtime_error
	{
	public:
		ConfigException(std::string error);
	};

	int getPort();
	static void	extractConfigMap(std::ifstream &conFile, std::map<std::string, std::string> &targetMap, std::string target);
	static int	checkConfigFile(std::ifstream &conFile);

protected:
	int			_port;
	std::string	_host;
	std::string	_root;
	std::string _index;
};

#endif //CONFIG_HPP