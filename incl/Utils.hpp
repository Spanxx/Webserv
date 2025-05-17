
#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <sstream>

std::string trim(const std::string &str);
const std::string intToString(int num);
void replaceAll(std::string &str, const std::string &placeholder, const std::string &goal);

#endif
