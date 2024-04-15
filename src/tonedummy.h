#pragma once

#include "tone.h"

class ToneDummy : public Tone {
public:
	virtual ~ToneDummy() {}

	virtual int getNotifyFD() const final { return -1; }
	virtual unsigned getFreq() const final { return 1000; }
};
