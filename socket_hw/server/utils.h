#pragma once

#include <iostream>
#include <vector>
#include <string>

#include <sys/stat.h>
#include <dirent.h>

// String
bool string_in(const std::string orig, const std::string search);
std::string strip(const std::string &orig, const std::string &search);
std::string rstrip(const std::string &orig, const std::string &search);
std::vector<std::string>& split(std::vector<std::string> &v, const std::string orig, const std::string delim=" ");

// Filesystem
bool checkFileExists(const std::string &path);
std::vector<std::string>& list_dir(std::vector<std::string> &v, const std::string &path);

// pack and unpack
// TODO(roy4801): pack() and unpack() funcs just like python has
void packi32(char *buf, unsigned long int i);
long int unpacki32(char *buf);