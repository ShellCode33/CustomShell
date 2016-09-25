#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include <sys/stat.h>
#include <dirent.h>
#include <cstring>

class Utils
{
public:
    Utils();
    static bool isDir(const char* path);
    static bool isDir(std::string path);
    static std::string compare(std::string s1, std::string s2);
    static std::vector<std::string> getDirFiles(std::string path);
    static std::string clearEscapedString(std::string str);
    static std::string escapeString(std::string str);
};

#endif // UTILS_H
