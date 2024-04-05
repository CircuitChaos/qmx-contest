#pragma once

#include <string>
#include <stdexcept>
#include "util.h"

/* Just prints error -- to be used in threads */
#define xprint(...)                                                 \
	do {                                                            \
		Exception e(__FILE__, __LINE__, util::format(__VA_ARGS__)); \
		e.print();                                                  \
	} while(0)

/* Throws an exception */
#define xthrow(...)                                                     \
	do {                                                                \
		throw Exception(__FILE__, __LINE__, util::format(__VA_ARGS__)); \
	} while(0)

/* Throws an exception if condition is false */
#define xassert(e, ...)          \
	do {                         \
		if(!(e))                 \
			xthrow(__VA_ARGS__); \
	} while(0)

class Exception {
public:
	Exception(const char *file, int line, const std::string &msg);
	void print() const;

private:
	std::string m_file;
	int m_line;
	std::string m_msg;
};
