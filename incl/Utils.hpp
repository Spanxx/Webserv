
#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <sstream>

std::string trim(const std::string &str);

const std::string intToString(int num);

int strToInt(std::string &value);

void replaceAll(std::string &str, const std::string &placeholder, const std::string &goal);

//	### Server Utils ###

std::string checkFilePath(char *av);

#endif
