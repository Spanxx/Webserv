
#include "../incl/Utils.hpp"
#include <cctype>  // for isxdigit
#include <cstdio>  // for sscanf

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

int strToInt(std::string &value)
{
	int	num = 0;
	std::stringstream oss;
	oss << value;
	oss >> num;

	return (num);
}

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
                		if (is_valid_hex(hex, val))
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

bool is_valid_hex(const std::string& str, int& value)
{
	std::istringstream iss(str);
	iss >> std::hex >> value;

	return !iss.fail() && iss.eof();
}
