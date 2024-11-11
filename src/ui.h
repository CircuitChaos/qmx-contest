#pragma once

#include <string>
#include "file.h"

class Ui {
public:
	Ui();
	~Ui();

	enum EventType {
		EVT_NONE,
		EVT_GAIN_UP,        /* arrow up */
		EVT_GAIN_DOWN,      /* arrow down */
		EVT_FREQ_UP_SLOW,   /* arrow right */
		EVT_FREQ_DOWN_SLOW, /* arrow left */
		EVT_FREQ_UP_FAST,   /* pgup */
		EVT_FREQ_DOWN_FAST, /* pgdn */
		EVT_FREQ_RESET,     /* = */
		EVT_WPM_UP,         /* u: keyer speed up */
		EVT_WPM_DOWN,       /* d: keyer speed down */
		EVT_QUIT,           /* q */
		EVT_SHOW_PRESETS,   /* p */
		EVT_SEND_PRESET,    /* 0...9 (data = '0' to '9') */
		EVT_ABORT_SENDING,  /* a */
		EVT_LOG,            /* l: log QSO; data = callsign, space, received rst, space, received exchange */
		EVT_SEND_TEXT,      /* t: send entered text */
		EVT_XCHG,           /* x: print current exchange */
		EVT_MUTE,           /* m: mute / unmute */
		EVT_FREEZE_TIME,    /* freeze time before QSO logging, must occur before EVT_LOG */
	};

	struct Event {
		Event(EventType type)
		    : type(type) {}

		Event(EventType type, const std::string &data)
		    : type(type), data(data) {}

		const EventType type;
		const std::string data;
	};

	int getFD() const { return 0; }
	void print(const char *fmt, ...);
	Event getEvent();

private:
	enum Mode {
		MODE_CMD,  // reading single character (non-canonical mode)
		MODE_TEXT, // reading text to send
		MODE_LOG,  // reading callsign, optionally rst, and received exchange
	};

	Mode mode{MODE_CMD};
	std::string text; /* text being read in MODE_TEXT and MODE_LOG */

	void setMode(Mode newMode);
	void setKbdMode(bool canonical);

	Event getEventCmd(const char *buf, size_t sz);
	Event getEventText(const char *buf, size_t sz);
	void showHelp();
};
