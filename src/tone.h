#pragma once

class Tone {
public:
	virtual ~Tone() {}

	/* Signals readability when thread exits prematurely (for example,
	 * QMX has been disconnected); raise exception and call dtor then */
	virtual int getNotifyFD() const  = 0;
	virtual unsigned getFreq() const = 0;
};
