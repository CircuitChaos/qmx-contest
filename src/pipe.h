#pragma once

class Pipe {
public:
	Pipe();
	~Pipe();

	int getReadEnd() const;
	bool readChar(char *ch = nullptr);
	void writeChar(char ch = 'x');
	bool readData(void *buf, size_t sz);
	void writeData(const void *buf, size_t sz);
	void closeWriteEnd();

private:
	int pfd[2];
};

class PipeGuard {
public:
	PipeGuard(Pipe &pipe);
	~PipeGuard();

private:
	Pipe &pipe;
};
