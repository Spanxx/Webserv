
#include "../incl/Utils.hpp"
#include <cctype>  // for isxdigit
#include <cstdio>  // for sscanf
#include <cerrno>
#include <climits>

std::string trim(const std::string &str)
{
	size_t start = str.find_first_not_of(" \t\r\n");
	size_t end = str.find_last_not_of(" \t\r\n");
	if (start == std::string::npos || end == std::string::npos)
		return "";
	return str.substr(start, end - start + 1);
}

const std::string intToString(int num)
{
	std::ostringstream oss;
	oss << num;
	return oss.str();
}

// int strToInt(std::string &value)
// {
// 	int	num = 0;
// 	std::stringstream oss;
// 	oss << value;
// 	oss >> num;

// 	return (num);
// }

void replaceAll(std::string &str, const std::string &placeholder, const std::string &goal)
{
	size_t start = 0;
	while((start = str.find(placeholder, start)) != std::string::npos)
	{
		str.replace(start, placeholder.length(), goal);
		start += goal.length();
	}
}


std::string urlDecode(const std::string &str)
{
	std::string result;
	size_t i = 0;
	while (i < str.length())
	{
		if (str[i] == '%')
		{
			if (i + 2 < str.length())
			{
				std::string hex = str.substr(i + 1, 2);
				int val;
				if (isValidHex(hex, val))
				{
					result += static_cast<char>(val);
					i += 3;
				}
			}
			else
			{
				// malformed %
 				result += str[i++];
				return "";
			}
		}
		else if (str[i] == '+') // '+' in URL encoding means space, so convert it
		{
			result += ' ';
			++i;
		}
		else
			result += str[i++]; // Normal character, just append
	}
	return result;
}

bool isValidHex(const std::string& str, int& value)
{
	std::istringstream iss(str);
	iss >> std::hex >> value;

	return !iss.fail() && iss.eof();
}

bool isValidIP(std::string &host)
{
	std::istringstream iss(host);
	std::string token;
	std::vector<int> parts;

	while (std::getline(iss, token, '.'))
	{
		int num;
		if (!safeAtoi(token, num) || (num < 0 || num > 255 || (num == 0 && token != "0")))
			return false;
		parts.push_back(num);
	}
	if (parts.size() != 4)
		return false;
	
	//10.0.0.0 /8 and 172.0.0.0 /8 and 192.168.0.0 /16
	if (parts[0] == 10 || parts[0] == 172 || (parts[0] == 192 && parts[1] == 168))
		return true;
	
	return false;
}

bool safeAtoi(const std::string& s, int& result)
{
	errno = 0;
	char* end;
	long val = std::strtol(s.c_str(), &end, 10);

	if (errno == ERANGE || val > INT_MAX || val < INT_MIN || *end != '\0')
		return false;

	result = static_cast<int>(val);
	return true;
}
