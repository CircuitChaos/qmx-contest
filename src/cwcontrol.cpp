#include "cwcontrol.h"
#include "exception.h"

CWControl::CWControl(QMXCat &cat, unsigned toneFreq, bool qsk)
    : cat(cat), toneFreq(toneFreq), qsk(qsk)
{
}

CWControl::~CWControl()
{
	try {
		if(mode != MODE_STOPPED) {
			stop();
		}
	}
	catch(const Exception &e) {
		e.print();
	}
}

void CWControl::start()
{
	xassert(mode == MODE_STOPPED, "start() in invalid mode (%d)", mode);
	mode    = MODE_STARTED;
	vfoFreq = cat.getFreq();

	if(!qsk) {
		cat.setFreq(vfoFreq - toneFreq);
		cat.setCW(false);
	}
}

void CWControl::stop()
{
	if(mode == MODE_KEYDOWN) {
		keyUp();
	}

	xassert(mode == MODE_STARTED, "stop() in invalid mode (%d)", mode);
	xassert(vfoFreq, "VFO frequency invalid in stop()");
	mode = MODE_STOPPED;

	if(!qsk) {
		cat.setCW(true);
		cat.setFreq(vfoFreq);
	}

	vfoFreq = 0;
}

void CWControl::keyDown()
{
	xassert(mode == MODE_STARTED, "keyDown() in invalid mode (%d)", mode);
	xassert(vfoFreq, "VFO frequency invalid in keyDown()");
	mode = MODE_KEYDOWN;

	if(qsk) {
		cat.setFreq(vfoFreq - toneFreq);
		cat.setCW(false);
	}

	cat.setTX(true);
}

void CWControl::keyUp()
{
	xassert(mode == MODE_KEYDOWN, "keyUp() in invalid mode (%d)", mode);
	xassert(vfoFreq, "VFO frequency invalid in keyUp()");
	mode = MODE_STARTED;

	cat.setTX(false);

	if(qsk) {
		cat.setCW(true);
		cat.setFreq(vfoFreq);
	}
}
