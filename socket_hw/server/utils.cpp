#include "utils.h"

// TODO(roy4801): may use set<>?
bool string_in(const std::string &orig, const std::string &search)
{
	auto pos = orig.find_first_of(search);
	return pos != std::string::npos;
}

// Not like python
// These *strip will strip from start to the first occurence of search
std::string lstrip(const std::string &orig, const std::string &search)
{
    size_t pos = orig.find_first_of(search);
    return orig.substr(pos+1, orig.size()-pos);
}

std::string rstrip(const std::string &orig, const std::string &search)
{
    size_t pos = orig.find_last_of(search);
    return orig.substr(0, pos);
}

std::string strip(const std::string &orig, const std::string &search)
{
	return rstrip(lstrip(orig, search), search);
}

std::vector<std::string>& split(std::vector<std::string> &v, const std::string orig, const std::string delim)
{
    int cur = 0;
    int idx = orig.find_first_of(delim, cur);
    while(idx != std::string::npos)
    {
        if(idx != cur)
            v.push_back(orig.substr(cur, idx-cur));
        cur = idx + 1;
        idx = orig.find_first_of(delim, cur);
    }
    if(cur < orig.size() && idx == std::string::npos)
        v.push_back(orig.substr(cur, orig.size()-cur));
    return v;
}

// Filesystem
bool checkFileExists(const std::string &path)
{
    struct stat res;
    if(stat(path.c_str(), &res) < 0)
    {
        printf("No such file or dictionary: %s", path.c_str());
        return false;
    }
    return true;
}

std::vector<std::string>& list_dir(std::vector<std::string> &v, const std::string &path)
{
    struct dirent *de;
    DIR *dir = opendir(path.c_str());
    if(!dir)
    {
        printf("Failed to open directory\n");
        goto END;
    }
    while((de = readdir(dir)) != NULL)
    {
        v.push_back(std::string(de->d_name));
    }
END:
    return v;
}

// pack and unpack
void packi32(char *buf, unsigned long int i)
{
    *buf++ = i>>24; *buf++ = i>>16;
    *buf++ = i>>8;  *buf++ = i;
}
long int unpacki32(char *buf)
{
    unsigned long int i2 = ((unsigned long int)buf[0]<<24) |
                           ((unsigned long int)buf[1]<<16) |
                           ((unsigned long int)buf[2]<<8)  |
                           buf[3];
    long int i;

    // change unsigned numbers to signed
    if (i2 <= 0x7fffffffu) { i = i2; }
    else { i = -1 - (long int)(0xffffffffu - i2); }

    return i;
}