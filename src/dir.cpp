#include "dir.h"
#include "exception.h"

Dir::Dir(const std::string &path)
{
	dir = opendir(path.c_str());
	xassert(dir, "Could not open directory %s: %m", path.c_str());
}

Dir::~Dir()
{
	closedir(dir);
}

std::string Dir::operator()()
{
	dirent *de(readdir(dir));
	return de ? de->d_name : "";
}
