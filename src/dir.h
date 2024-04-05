#pragma once

#include <string>
#include <sys/types.h>
#include <dirent.h>

class Dir {
public:
	Dir(const std::string &path);
	~Dir();

	std::string operator()();

private:
	DIR *dir;
};
