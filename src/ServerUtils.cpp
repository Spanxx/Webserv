#include "../incl/Server.hpp"

std::string checkFilePath(char *av)
{
	std::string filePath;

	if (av != NULL)
		filePath = av;

	std::ifstream f1(filePath.c_str());
	if (f1.good())
		return (filePath);

	std::ifstream f2(("www/config/" + filePath).c_str());
	if (f2.good())
		return ("www/config/" + filePath);

	std::ifstream f3(("www/" + filePath).c_str());
	if (f3.good())
		return ("www/" + filePath);

	std::string fallback = "www/config/server.conf";
	std::ifstream f4(fallback.c_str());
	if (f4.good())
		return fallback;

	return (filePath);
}
