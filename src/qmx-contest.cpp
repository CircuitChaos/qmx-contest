#include <unistd.h>
#include "exception.h"
#include "qmxdetector.h"
#include "qmxtone.h"
#include "qmxcat.h"
#include "util.h"
#include "cli.h"
#include "keyer.h"
#include "cwcontrol.h"
#include "cabrillo.h"
#include "presets.h"
#include "ui.h"

static void main2(int ac, char *const av[])
{
	Cli cli(ac, av);
	if(cli.getExitFlag()) {
		return;
	}

	Presets presets(cli.getCallsign());
	// TODO parametrize VID and PID in CLI
	// TODO some thread reading QMX audio and displaying intensity in graphical form for easier decoding by eye
	unsigned wpm(cli.getInitialWPM());
	QMXCat cat(cli.getCatDev().empty() ? detectQMXPort() : cli.getCatDev());
	QMXTone tone(cli.getAlsaDev().empty() ? detectQMXAlsaDev() : cli.getAlsaDev());
	CWControl cwctl(cat, tone.getFreq(), cli.getQSK());
	std::string exchange(cli.getExchange());
	Keyer keyer(wpm);
	Ui ui;
	const std::set<int> fds{cat.getFD(), tone.getNotifyFD(), keyer.getFD(), ui.getFD()};

	const bool interactive(cli.getTextToSend().empty());
	if(!interactive) {
		ui.print("Sending text: %s", cli.getTextToSend().c_str());
		ui.print("Press 'a' to abort");
		keyer.sendText(cli.getTextToSend());
	}
	else {
		ui.print("Press 'h' for help, 'q' to quit");
	}

	bool exitFlag(false);
	while(!exitFlag) {
		const std::set<int> outfds(util::watch(fds, -1));

		if(util::inSet<int>(outfds, tone.getNotifyFD())) {
			xthrow("QMX audio interface disconnected");
		}

		if(util::inSet<int>(outfds, cat.getFD())) {
			xthrow("QMX either sent something via CAT unexpectedly, or serial interface was disconnected");
		}

		if(util::inSet<int>(outfds, keyer.getFD())) {
			switch(keyer.getEvent()) {
				case Keyer::EVT_NONE:
					break;

				case Keyer::EVT_START:
					cwctl.start();
					break;

				case Keyer::EVT_KEY_DOWN:
					cwctl.keyDown();
					break;

				case Keyer::EVT_KEY_UP:
					cwctl.keyUp();
					break;

				case Keyer::EVT_STOP:
					cwctl.stop();
					ui.print("Sending done");

					if(!interactive) {
						exitFlag = true;
					}

					break;

				default:
					xthrow("Invalid event from keyer");
			}
		}

		if(util::inSet<int>(outfds, ui.getFD())) {
			const Ui::Event evt(ui.getEvent());

			switch(evt.type) {
				case Ui::EVT_NONE:
					break;

				// TODO what's the maximum here?
				case Ui::EVT_GAIN_UP: {
					unsigned gain(cat.getGain());
					if(gain < 999) {
						gain++;
						ui.print("Set gain to %u dB", gain);
						cat.setGain(gain);
					}
					else {
						ui.print("Gain %u dB is already maximum", gain);
					}
					break;
				}

				// TODO what's the minimum here?
				case Ui::EVT_GAIN_DOWN: {
					unsigned gain(cat.getGain());
					if(gain > 0) {
						gain--;
						ui.print("Set gain to %u dB", gain);
						cat.setGain(gain);
					}
					else {
						ui.print("Gain %u dB is already minimum", gain);
					}
					break;
				}

				// TODO redo freq, add some limits or rollbacks, do it smarter
				// TODO don't touch freq when keyer is active
				case Ui::EVT_FREQ_UP_SLOW: {
					const uint32_t freq(cat.getFreq() + 10);
					ui.print("Set freq to %u.%03u kHz", freq / 1000, freq % 1000);
					cat.setFreq(freq);
					break;
				}

				case Ui::EVT_FREQ_DOWN_SLOW: {
					const uint32_t freq(cat.getFreq() - 10);
					ui.print("Set freq to %u.%03u kHz", freq / 1000, freq % 1000);
					cat.setFreq(freq);
					break;
				}

				case Ui::EVT_FREQ_UP_FAST: {
					const uint32_t freq(cat.getFreq() + 1000);
					ui.print("Set freq to %u.%03u kHz", freq / 1000, freq % 1000);
					cat.setFreq(freq);
					break;
				}

				case Ui::EVT_FREQ_DOWN_FAST: {
					const uint32_t freq(cat.getFreq() - 1000);
					ui.print("Set freq to %u.%03u kHz", freq / 1000, freq % 1000);
					cat.setFreq(freq);
					break;
				}

				case Ui::EVT_WPM_UP:
					if(wpm < 99) {
						wpm++;
						ui.print("Set local keyer speed to %u WPM", wpm);
						keyer.setWPM(wpm);
					}
					else {
						ui.print("Local keyer WPM already maximum");
					}
					break;

				case Ui::EVT_WPM_DOWN:
					if(wpm > 1) {
						wpm--;
						ui.print("Set local keyer speed to %u WPM", wpm);
						keyer.setWPM(wpm);
					}
					else {
						ui.print("Local keyer WPM already minimum");
					}
					break;

				case Ui::EVT_QUIT:
					exitFlag = true;
					break;

				case Ui::EVT_SHOW_PRESETS:
					for(size_t i(0); i < presets.numPresets(); ++i) {
						ui.print("Preset %d: %s", (i + 1) % 10, presets.getPreset(i, exchange).c_str());
					}
					break;

				case Ui::EVT_SEND_PRESET: {
					xassert(evt.data.size() == 1 && evt.data[0] >= '0' && evt.data[0] <= '9', "Invalid event received");
					unsigned presetNo(evt.data[0] - '0');
					if(presetNo == 0) {
						presetNo = 10;
					}

					if(presetNo > presets.numPresets()) {
						ui.print("Preset %d not found", presetNo % 10);
					}
					else {
						// TODO maybe concatenate?
						if(keyer.busy()) {
							ui.print("Cannot send, keyer busy; abort first");
						}
						else {
							const std::string preset(presets.getPreset(presetNo - 1, exchange));
							ui.print("Sending preset: %s", preset.c_str());
							keyer.sendText(preset);
						}
					}
					break;
				}

				case Ui::EVT_ABORT_SENDING:
					if(keyer.busy()) {
						keyer.stop();
					}
					break;

				case Ui::EVT_LOG: {
					const std::vector<std::string> v(util::tokenize(evt.data, " ", 0));
					xassert(v.size() == 2 || v.size() == 3, "Invalid data received in UI event: %s", evt.data.c_str());

					CabrilloQSO qso;
					qso.freq         = cat.getFreq();
					qso.sentCall     = cli.getCallsign();
					qso.sentRST      = "599"; // TODO this is fixed for now
					qso.sentExchange = exchange;
					qso.rcvdCall     = v[0];
					qso.rcvdRST      = (v.size() == 3) ? v[1] : "599";
					qso.rcvdExchange = v[(v.size() == 3) ? 2 : 1];
					ui.print("%s", logCabrillo(cli.getCbrFile(), qso).c_str());

					if(!cli.isExchangeFixed()) {
						exchange = util::format(util::format("%%0%zulu", exchange.size()).c_str(), strtol(exchange.c_str(), NULL, 10) + 1);
						ui.print("Next exchange is: %s", exchange.c_str());
					}
					break;
				}

				case Ui::EVT_XCHG:
					ui.print("Exchange to send to the other party: %s", exchange.c_str());
					break;

				case Ui::EVT_SEND_TEXT:
					if(keyer.busy()) {
						ui.print("Cannot send, keyer busy; abort first");
					}
					else {
						ui.print("Sending text: %s", evt.data.c_str());
						keyer.sendText(evt.data);
					}
					break;

				default:
					xthrow("Unknown event %u received from UI", evt.type);
			}
		}
	}

	ui.print("QRT");
}

int main(int ac, char *const av[])
{
	try {
		main2(ac, av);
	}
	catch(const Exception &e) {
		e.print();
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
