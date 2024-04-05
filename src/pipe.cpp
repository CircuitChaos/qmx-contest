#include <unistd.h>
#include "exception.h"
#include "pipe.h"

Pipe::Pipe()
{
	xassert(pipe(pfd) == 0, "pipe(): %m");
}

Pipe::~Pipe()
{
	if(pfd[1] != -1) {
		close(pfd[1]);
	}

	if(pfd[0] != -1) {
		close(pfd[0]);
	}
}

bool Pipe::readChar(char *ch)
{
	char buf;
	const ssize_t rs(read(pfd[0], &buf, 1));
	if(rs <= 0) {
		return false;
	}

	if(ch) {
		*ch = buf;
	}

	return true;
}

void Pipe::writeChar(char ch)
{
	write(pfd[1], &ch, 1);
}

void Pipe::writeData(const void *buf, size_t sz)
{
	write(pfd[1], buf, sz);
}

bool Pipe::readData(void *buf, size_t sz)
{
	const ssize_t rs(read(pfd[0], buf, sz));
	return rs == (ssize_t) sz;
}

void Pipe::closeWriteEnd()
{
	if(pfd[1] != -1) {
		close(pfd[1]);
		pfd[1] = -1;
	}
}

int Pipe::getReadEnd() const
{
	return pfd[0];
}

PipeGuard::PipeGuard(Pipe &pipe)
    : pipe(pipe)
{
}

PipeGuard::~PipeGuard()
{
	pipe.closeWriteEnd();
}
