
#include	"../incl/Server.hpp"

void	Server::extractVariables()
{
	std::map<std::string, std::string> *config = getConfigMap("serverConfig");

	if (!config)
		throw ServerException("Extracting serverConfig map failed!");

	std::map<std::string, std::string>::iterator it = config->begin();
	while (it != config->end())
	{
		if (it->first.find("listen") != std::string::npos)
			extractPorts(it->second);
		else if (it->first.find("host") != std::string::npos)
			extractHost(it->second);
		else if (it->first.find("name") != std::string::npos)
			extractName(it->second);
		else if (it->first.find("root") != std::string::npos)
			extractRoot(it->second);
		else if (it->first.find("maxbodysize") != std::string::npos)
			extractMaxBody(it->second);
		else if (it->first.find("errorPage") != std::string::npos)
			extractErrorPage(it->second);
		++it;
	}
	checkCompletes();
}

void	Server::extractPorts(const std::string &ports)
{
	int			port = 0;
	int			portCounter = 0;
	std::string item;
	std::stringstream ss(ports);

	while (std::getline(ss, item, ','))
	{
		if (!safeAtoi(item, port) || port < 1024 || port > 65535) //below 1024 only with sudo rights
			throw ServerException("Ports need to be between 1024 and 65535");
		this->_ports.push_back(port);
		++portCounter;
	}
	this->_numPorts = portCounter;

	std::vector<int>::iterator iPorts = this->_ports.begin();
	while (iPorts != this->_ports.end())
	{
		std::cout << "Port: " << *iPorts << '\n';	// add  check for port duplicates
		++iPorts;
	}
}

void	Server::extractHost(const std::string &host)
{
	if (!isValidIP(host))
			throw ServerException("Host IP needs to be within a private or loopback range");
	this->_IPHost = host;
	std::cout << "Host: " << this->_IPHost << '\n';
}

void	Server::extractMaxBody(const std::string &maxbody)
{
	int size;
	if (!safeAtoi(maxbody, size) || size < 0)
		throw ServerException("Max body size needs to be between 0 and INT MAX");
	_maxBodySize = static_cast<size_t>(size);
}

void	Server::extractErrorPage(const std::string &path)
{
	_errorPage = path;
	std::ifstream file(_errorPage.c_str());
	if (!file.is_open())
		throw ServerException("Error page file does not exist or is not readable");
	file.close();
}

void	Server::extractName(const std::string &name)
{
	this->_name = name;
	std::cout << "Server Name: " << this->_name << '\n';
}

void	Server::extractRoot(const std::string &root)
{
	if (root.find("www/") == std::string::npos)
		throw ServerException("Root path usage: www/<ServerName>\n");
	if (root.find(this->_name) == std::string::npos)
		throw ServerException("Root path usage: www/<ServerName>\n");
	
	this->_serverRoot = root;
}

void	Server::checkCompletes()
{
	if (_errorPage.empty())
		throw ServerException("Config file needs to specify an error page file");
	if (_IPHost.empty())
	{
		std::cout << "No host in config file, default set to bind to any local address\n";
		this->_IPHost = "0.0.0.0"; // bind to any local address
	}
	if (_name.empty())
	{
		std::cout << "No name in config file, default set to defaultServer!\n";
		this->_name = "default_server"; // default server name
	}
	if (_ports.empty())
		throw ServerException("Config file needs to specify port per server");
}
