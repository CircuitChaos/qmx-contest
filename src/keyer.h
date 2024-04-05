#include <thread>
#include <vector>
#include <utility>
#include <string>
#include "pipe.h"

class Keyer {
public:
	Keyer(unsigned defaultWPM);
	~Keyer();

	enum Event {
		EVT_NONE,
		EVT_START,
		EVT_KEY_DOWN,
		EVT_KEY_UP,
		EVT_STOP,
	};

	void setWPM(unsigned newWPM);
	void sendText(const std::string &s);
	void stop();
	bool busy() const;
	int getFD() const;
	Event getEvent();

private:
	std::thread thread;
	unsigned wpm;
	// TODO it would be better with socketpair
	Pipe cmdPipe, rspPipe;
	std::vector<std::pair<unsigned, Event> > pending;

	void scheduleTimer(unsigned usec);
	void next();
	void addPending(unsigned dotTimes, Event evt);

	static std::string stringToMorse(const std::string &s);
};
