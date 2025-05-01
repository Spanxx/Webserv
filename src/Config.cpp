
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
			else if (it->first == "host")
				this->_host = std::atoi(it->second.c_str());
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
