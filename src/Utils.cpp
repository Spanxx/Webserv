
#include "../incl/Utils.hpp"

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

void replaceAll(std::string &str, const std::string &placeholder, const std::string &goal)
{
	size_t start = 0;
	while((start = str.find(placeholder, start)) != std::string::npos)
	{
		str.replace(start, placeholder.length(), goal);
		start += goal.length();
	}
}
