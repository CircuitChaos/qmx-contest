#pragma once

#include <cstdio>

class File {
public:
	File(FILE *fp);
	~File();

	operator FILE *() const;

private:
	FILE *fp;
};

class Fd {
public:
	Fd(int fd);
	~Fd();

	operator int() const;

private:
	int fd;
};
