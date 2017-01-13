#include "utils.h"

using namespace std;

Utils::Utils()
{

}

bool Utils::fileExists(const char* path)
{
    struct stat buffer;
    return (stat (path, &buffer) == 0);
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

string Utils::escapeString(string str)
{
    int len = str.size();
    for(int i = 1; i < len; i++)
    {
        if(str[i] == ' ' && str[i-1] != '\\')
        {
            str.insert(i, 1, '\\');
            i++;
            len++;
        }
    }

    return str;
}

vector<string> Utils::parse(string line)
{
    vector<string> words;

    int i = 0;
    while(line[i] == ' ') //skip spaces
        i++;

    int begin_index = i;

    bool stringBegin = false; // " not found

    for(; i < (int)line.size(); i++)
    {
        char test = line[i];
        if(line[i] == '"') //match "
        {
            cout << stringBegin << endl;
            stringBegin = !stringBegin;
        }

        if(line[i] == ' ' && line[i-1] != '\\' && !stringBegin)
        {
            string word = line.substr(begin_index, i-begin_index);

            if(word[word.size()-1] == '"')
                word.erase(word.begin() + word.size() -1);

            if(word[0] == '"')
                word.erase(word.begin());

            words.push_back(word);

            while(line[i] == ' ') //skip spaces
                i++;

            begin_index = i--; //because the for loop will i++ so i--
        }
    }

    if(begin_index < (int)line.size())
    {
        string word = line.substr(begin_index);

        if(word[word.size()-1] == '"')
            word.erase(word.begin() + word.size() -1);

        if(word[0] == '"')
            word.erase(word.begin());

        words.push_back(word);
    }


    return words;
}
