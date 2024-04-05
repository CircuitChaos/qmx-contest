#include <unistd.h>
#include "file.h"

File::File(FILE *fp)
    : fp(fp)
{
}

File::~File()
{
	if(fp) {
		fclose(fp);
	}
}

File::operator FILE *() const
{
	return fp;
}

Fd::Fd(int fd)
    : fd(fd)
{
}

Fd::~Fd()
{
	if(fd != -1) {
		close(fd);
	}
}

Fd::operator int() const
{
	return fd;
}
