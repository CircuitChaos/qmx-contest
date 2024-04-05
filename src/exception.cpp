#include <cstdio>
#include "exception.h"

Exception::Exception(const char *file, int line, const std::string &msg):
	m_file(file),
	m_line(line),
	m_msg(msg)
{
}

void Exception::print() const
{
	fprintf(stderr, "%s:%d: %s\n", m_file.c_str(), m_line, m_msg.c_str());
}
