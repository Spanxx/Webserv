#include "../incl/Router.hpp"
#include "../incl/Request.hpp"
#include "../incl/Utils.hpp"

void	Request::check_headers(const std::string &headers_raw)
{
	std::istringstream rstream(headers_raw); //turn string into stream so it can be read line by line with getline
	std::string line;
	std::cout << "HEADERS RAW: " << headers_raw << "UNTIL HERE" << std::endl;
	if (std::getline(rstream, line))
	{
		std::istringstream lstream(line); //splits with space as delimiter
		std::string extra;
		if (!(lstream >> _method >> _path >> _version) || lstream >> extra) // less than or more than 3 parts
		{
			this->_code = 400;
			return;
		}
	}
	// make extra check for header too long for buffer --> code 431
	// URI to long
	_path = urlDecode(_path);
	if (_path.empty())
	{
		this->_code = 400;
		return;
	}
	splitURI();
	std::cout << "PATH: " << _path << ", QUERY: " << _query << std::endl;

	if (split_headers(rstream) == 1)
		return ;
	if (checkURILength() == 1)
		return ;
	if (checkPathChars() == 1)
		return;
	// if (checkRequestedFiletype() == 1)
	// 	return ;

	Router Router(this->_server, this);
	//this->_code = 200;
}

int Request::split_headers(std::istringstream &rstream)
{
	std::string line;
	bool blank = false;
	// _content_length = -1;
	// _chunked = false;
	while (std::getline(rstream, line))
	{
		if (!line.empty() && line[line.size() - 1] == '\r') //getline removes \n but not \r
			line.erase(line.size() - 1); //remove last char (trailing carriage return '\r')
		if (line.empty())	//if there is no body, then there is maybe no empty line
		{
			blank = true;
			break;
		}
		size_t pos = line.find(": ");
		if (pos == std::string::npos || line[pos - 1] == ' ') //if no colon or extra whitespace
			return 0;
		std::string key = line.substr(0, pos);
		std::string value = line.substr(pos + 2);
		trim(key);
		trim(value);
		// std::cout << "key: " << key << " // value: " << value << '\n';	//header values for debugging
		if (key.empty() || value.empty())
			return 0;
		_headers[key] = value;
		if (key == "Content-Length")
		{
			_content_length = std::atoi(value.c_str());
			std::cout << "Content-Length: " << _content_length << "*****" << std::endl;
		}
		if (key == "Transfer-Encoding" && value == "chunked")
			_chunked = true;
	}

	if (!blank)
	{
		this->_code = 400;
		return (1); //no empty line after header
	}
	if (_content_length == -1 && _method != "POST") //check if only in GET and DELETE it's ok not to have content_length set
		_content_length = 0;
	return 0;
}
