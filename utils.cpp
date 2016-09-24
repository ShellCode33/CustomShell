#include "utils.h"

using namespace std;

Utils::Utils()
{

}

bool Utils::isDir(const char* path)
{
    struct stat buf;
    stat(path, &buf);
    return S_ISDIR(buf.st_mode);
}

bool Utils::isDir(string path)
{
    return isDir(path.c_str());
}

//retourne le début "commun" de 2 strings
string Utils::compare(string s1, string s2)
{
    string common = "";

    int i = 0;
    while(s1[i] != '\0' && s1[i] == s2[i])
        common += s1[i++];

    return common;
}

vector<string> Utils::getDirFiles(string path)
{
    path = clearEscapedString(path);
    vector<string> files;

    DIR *dir = opendir(path.c_str());
    struct dirent *item;

    if(dir != NULL)
    {
        while ((item = readdir(dir)) != NULL)
            if(strcmp(item->d_name, "..") != 0 && strcmp(item->d_name, ".") != 0)
                files.push_back(string(item->d_name));

        closedir (dir);
    }
/*
    else
        cout << "Impossible de lister le contenu du repertoire." << endl;
*/
    return files;
}

string Utils::clearEscapedString(string str)
{
    string result(str);
    size_t retFind = result.find("\\");

    while(retFind != string::npos)
    {
        result.erase(retFind, 1);
        retFind = result.find("\\");
    }

    return result;
}


