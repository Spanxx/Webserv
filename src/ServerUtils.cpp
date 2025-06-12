#include "../incl/Server.hpp"

std::string checkFilePath(char *av)
{
	std::string filePath;
	std::string tryPath;

	if (av != NULL)
		filePath = av;

	std::ifstream f1(filePath.c_str());
	if (f1.good())
		return (filePath);

	tryPath = "www/config/" + filePath;
	std::ifstream f2(tryPath.c_str());
	if (f2.good())
		return (tryPath);

	tryPath = "www/" + filePath;
	std::ifstream f3(tryPath.c_str());
	if (f3.good())
		return (tryPath);

	std::string fallback = "www/config/default.conf";
	std::ifstream f4(fallback.c_str());
	if (f4.good())
		return fallback;

	return ("");
}
