
#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <sstream>

std::string trim(const std::string &str);

const std::string intToString(int num);

//int strToInt(std::string &value);

void replaceAll(std::string &str, const std::string &placeholder, const std::string &goal);

//	### Server Utils ###

std::string checkFilePath(char *av);
std::string urlDecode(const std::string &str);
bool isValidHex(const std::string& str, int& value);
bool isValidIP(std::string &host);
bool safeAtoi(const std::string& s, int& result);

std::vector<std::string> parseMultipartBody(std::string& body, const std::string& boundary);
std::string extractFileContentFromPart(std::string& part);
std::string extractFilenameFromPart(std::string& part);

#endif
