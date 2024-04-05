#include <map>
#include <unistd.h>
#include <ctype.h>
#include "exception.h"
#include "keyer.h"

/* How this works:
 *
 * - sendText() converts text to pending -- a sequence of:
 *   - time to wait
 *   - event to deliver
 * - next() reads next entry from pending and schedules timer
 * - getEvent() clears the pipe (reads it) and:
 *   - removes entry from vector calls next() if vector is not empty
 *   - returns event
 */

static void threadFunc(Pipe *cmdPipe, Pipe *rspPipe)
{
	PipeGuard pg(*rspPipe);

	for(;;) {
		unsigned usec;
		if(!cmdPipe->readData(&usec, sizeof(usec))) {
			break;
		}

		usleep(usec);
		rspPipe->writeChar();
	}
}

Keyer::Keyer(unsigned defaultWPM)
    : wpm(defaultWPM)
{
	thread = std::thread(threadFunc, &cmdPipe, &rspPipe);
}

Keyer::~Keyer()
{
	cmdPipe.closeWriteEnd();
	thread.join();
}

void Keyer::setWPM(unsigned newWPM)
{
	wpm = newWPM;
}

bool Keyer::busy() const
{
	return !pending.empty();
}

void Keyer::sendText(const std::string &s)
{
	const std::string morseString(stringToMorse(s));

	addPending(0, EVT_START);
	for(auto ch : morseString) {
		switch(ch) {
			case '-':
				/* dash: key down, 3x dot time, key up, 1x dot time */
				addPending(0, EVT_KEY_DOWN);
				addPending(3, EVT_KEY_UP);
				addPending(1, EVT_NONE);
				break;

			case '.':
				/* dot: key down, 1x dot time, key up, 1x dot time */
				addPending(0, EVT_KEY_DOWN);
				addPending(1, EVT_KEY_UP);
				addPending(1, EVT_NONE);
				break;

			case ' ':
				/* inter-character space: 2x dot time (because 1x was from previous symbol) */
				addPending(2, EVT_NONE);
				break;

			default:
				xthrow("Invalid char %c in morse string", ch);
		}
	}

	addPending(0, EVT_STOP);
	next();
}

void Keyer::stop()
{
	if(!pending.empty()) {
		pending.clear();
		addPending(0, EVT_STOP);
	}
}

int Keyer::getFD() const
{
	return rspPipe.getReadEnd();
}

Keyer::Event Keyer::getEvent()
{
	rspPipe.readChar();

	xassert(!pending.empty(), "Keyer event, but event not scheduled");
	const Event e(pending[0].second);
	pending.erase(pending.begin());

	if(!pending.empty()) {
		next();
	}

	return e;
}

void Keyer::scheduleTimer(unsigned usec)
{
	cmdPipe.writeData(&usec, sizeof(usec));
}

void Keyer::next()
{
	xassert(!pending.empty(), "next() called on empty pending vector");
	scheduleTimer(pending[0].first * (1000000 / (wpm * 50. / 60)));
}

std::string Keyer::stringToMorse(const std::string &s)
{
	static const std::map<char, std::string> map = {
	    {' ', " "},
	    {'a', ".-"},
	    {'b', "-..."},
	    {'c', "-.-."},
	    {'d', "-.."},
	    {'e', "."},
	    {'f', "..-."},
	    {'g', "--."},
	    {'h', "...."},
	    {'i', ".."},
	    {'j', ".---"},
	    {'k', "-.-"},
	    {'l', ".-.."},
	    {'m', "--"},
	    {'n', "-."},
	    {'o', "---"},
	    {'p', ".--."},
	    {'q', "--.-"},
	    {'r', ".-."},
	    {'s', "..."},
	    {'t', "-"},
	    {'u', "..-"},
	    {'v', "...-"},
	    {'w', ".--"},
	    {'x', "-..-"},
	    {'y', "-.--"},
	    {'z', "--.."},
	    {'1', ".----"},
	    {'2', "..---"},
	    {'3', "...--"},
	    {'4', "....-"},
	    {'5', "....."},
	    {'6', "-...."},
	    {'7', "--..."},
	    {'8', "---.."},
	    {'9', "----."},
	    {'0', "-----"},
	    {',', "..-.."},
	    {'.', ".-.-.-"},
	    {'?', "..--.."},
	    {';', "-.-.-"},
	    {':', "---..."},
	    {'/', "-..-."},
	    {'+', ".-.-."},
	    {'-', "-....-"},
	    {'=', "-...-"},
	};

	std::string rs;
	for(auto ch : s) {
		const auto it(map.find(tolower(ch)));
		if(it == map.end()) {
			// TODO log this
			continue;
		}

		if(!rs.empty()) {
			rs += " ";
		}

		rs += it->second;
	}

	return rs;
}

void Keyer::addPending(unsigned usec, Event evt)
{
	pending.push_back(std::pair<unsigned, Event>(usec, evt));
}
