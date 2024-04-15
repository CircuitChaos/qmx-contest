#include <unistd.h>
#include <termios.h>
#include <cstring>
#include <stdarg.h>
#include "exception.h"
#include "ui.h"

Ui::Ui()
{
	setKbdMode(false);
}

Ui::~Ui()
{
	setKbdMode(true);
}

void Ui::print(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vprintf(fmt, ap);
	va_end(ap);
	printf("\n");
	// TODO log it in some file with current date + time
}

Ui::Event Ui::getEvent()
{
	/* TODO: this might not work as expected if something slows the
	 * program down (we might get multiple keystrokes here and get
	 * confused)
	 *
	 * It would be best to use ncurses here.
	 */

	char buf[256];
	const ssize_t rs(read(0, &buf, sizeof(buf)));
	if(rs < 0) {
		if(errno == EAGAIN || errno == EINTR) {
			return EVT_NONE;
		}

		xthrow("Error reading from stdin");
	}

	if(rs == 0) {
		// EOF
		return EVT_QUIT;
	}

	xassert(rs <= (ssize_t) sizeof(buf), "read() returned nonsense (%zd)", rs);
	return (mode == MODE_CMD) ? getEventCmd(buf, rs) : getEventText(buf, rs);
}

Ui::Event Ui::getEventCmd(const char *buf, size_t sz)
{
	if(sz == 4) {
		if(!memcmp(buf, "\x1b\x5b\x35\x7e", 4)) {
			/* pgup */
			return EVT_FREQ_UP_FAST;
		}

		if(!memcmp(buf, "\x1b\x5b\x36\x7e", 4)) {
			/* pgdn */
			return EVT_FREQ_DOWN_FAST;
		}

		return EVT_NONE;
	}

	if(sz == 3) {
		if(!memcmp(buf, "\x1b\x5b\x41", 3)) {
			/* up */
			return EVT_GAIN_UP;
		}

		if(!memcmp(buf, "\x1b\x5b\x42", 3)) {
			/* down */
			return EVT_GAIN_DOWN;
		}

		if(!memcmp(buf, "\x1b\x5b\x44", 3)) {
			/* left */
			return EVT_FREQ_DOWN_SLOW;
		}

		if(!memcmp(buf, "\x1b\x5b\x43", 3)) {
			/* right */
			return EVT_FREQ_UP_SLOW;
		}

		return EVT_NONE;
	}

	if(sz != 1) {
		return EVT_NONE;
	}

	if(buf[0] >= '0' && buf[0] <= '9') {
		return Event(EVT_SEND_PRESET, std::string(1, buf[0]));
	}

	switch(buf[0]) {
		case 'a':
			return EVT_ABORT_SENDING;

		case 0x04: /* Ctrl-D */
		case 'q':
			return EVT_QUIT;

		case 'u':
			return EVT_WPM_UP;

		case 'd':
			return EVT_WPM_DOWN;

		case 'p':
			return EVT_SHOW_PRESETS;

		case 'x':
			return EVT_XCHG;

		case 'm':
			return EVT_MUTE;

		case 'l':
			print("Enter: <callsign> [<RST>] <exchange>");
			setMode(MODE_LOG);
			return EVT_FREEZE_TIME;

		case 't':
			print("Enter text to send");
			setMode(MODE_TEXT);
			break;

		case 'h':
			showHelp();
			break;

		default:
			break;
	}

	return EVT_NONE;
}

Ui::Event Ui::getEventText(const char *buf, size_t sz)
{
	// TODO this reinvents the wheel, we really should use ncurses here

	for(size_t i(0); i < sz; ++i) {
		if(buf[i] == 0x0d) {
			continue;
		}

		if(buf[i] == 0x0a) {
			/* The rest of buffer is ignored, but that's probably OK */
			const Mode m(mode);
			const std::string t(text);
			setMode(MODE_CMD);

			if(t.empty()) {
				print("No text entered -- aborting command");
				return EVT_NONE;
			}

			return Event((m == MODE_TEXT) ? EVT_SEND_TEXT : EVT_LOG, t);
		}

		/*
		if(buf[i] == 0x08) {
		    if(!text.empty()) {
		        text.erase(text.end() - 1);
		    }
		    continue;
		}

		if(buf[i] == 0x15) {
		    text.clear();
		    continue;
		}
		*/

		text += buf[i];
	}

	return EVT_NONE;
}

void Ui::setKbdMode(bool canonical)
{
	termios t;
	xassert(tcgetattr(0, &t) != -1, "Could not get stdin attributes: %m");

	if(canonical) {
		t.c_lflag |= ECHO | ICANON;
	}
	else {
		t.c_lflag &= ~(ECHO | ICANON);
		t.c_cc[VMIN]  = 1;
		t.c_cc[VTIME] = 0;
	}

	xassert(tcsetattr(0, TCSADRAIN, &t) != -1, "Could not set stdin attributes: %m");
}

void Ui::setMode(Mode newMode)
{
	mode = newMode;
	setKbdMode(mode != MODE_CMD);
	text.clear();
}

void Ui::showHelp()
{
	print("=== Keyboard help ===");
	print("");
	print("QMX control:");
	print("- up / down: adjust gain");
	print("- left / right: adjust VFO frequency (10 Hz steps)");
	print("- pgup / pgdn: adjust VFO frequency (1 kHz steps)");
	print("- m: mute / unmute");
	print("");
	print("Built-in keyer:");
	print("- u / d: adjust speed");
	print("- p: show presets");
	print("- 0...9: send preset");
	print("- t: send text");
	print("- a: abort sending");
	print("");
	print("Logging and contesting:");
	print("- x: show current exchange");
	print("- l: log QSO (time is registered when pressed");
	print("");
	print("Program control:");
	print("- h: show help (this screen)");
	print("- q: quit");
	print("");
	print("=== Keyboard help ===");
}
