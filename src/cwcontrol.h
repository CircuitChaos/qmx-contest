#pragma once

#include "qmxcat.h"

class CWControl {
public:
	CWControl(QMXCat &cat, unsigned toneFreq, bool qsk);
	~CWControl();

	void start();
	void stop();
	void keyDown();
	void keyUp();

private:
	enum Mode {
		MODE_STOPPED,
		MODE_STARTED,
		MODE_KEYDOWN,
	};

	QMXCat &cat;
	const unsigned toneFreq;
	const bool qsk;
	uint32_t vfoFreq{0};
	Mode mode{MODE_STOPPED};
};
