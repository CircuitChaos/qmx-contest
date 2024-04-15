#pragma once

#include <inttypes.h>

class Cat {
public:
	virtual ~Cat() {}

	virtual int getFD() const = 0;

	virtual unsigned getGain()          = 0;
	virtual void setGain(unsigned gain) = 0;

	virtual uint32_t getFreq()          = 0;
	virtual void setFreq(uint32_t freq) = 0;

	virtual bool isCW()         = 0;
	virtual void setCW(bool cw) = 0;

	virtual bool isTX()         = 0;
	virtual void setTX(bool tx) = 0;
};
