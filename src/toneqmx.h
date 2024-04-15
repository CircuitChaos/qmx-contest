#pragma once

#include <thread>
#include <string>
#include "tone.h"
#include "pipe.h"

class ToneQMX : public Tone {
public:
	ToneQMX(const std::string &alsaDev);
	virtual ~ToneQMX();

	virtual int getNotifyFD() const final;
	virtual unsigned getFreq() const final;

private:
	std::thread thread;
	Pipe notifyPipe;
	bool exitFlag{false};
};
