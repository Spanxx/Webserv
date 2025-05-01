
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

private:
	int			_port;
	std::string	_host;
	std::string	_root;
	std::string _index;
	// std::string	_tryFiles;
};

#endif //CONFIG_HPP