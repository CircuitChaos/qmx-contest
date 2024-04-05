#pragma once

#include <thread>
#include <string>
#include "pipe.h"

class QMXTone {
public:
	QMXTone(const std::string &alsaDev);
	~QMXTone();

	/* Signals readability when thread exits prematurely (for example,
	 * QMX has been disconnected); raise exception and call dtor then */
	int getNotifyFD() const;
	unsigned getFreq() const;

private:
	std::thread thread;
	Pipe notifyPipe;
	bool exitFlag{false};
};
