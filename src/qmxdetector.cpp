#include <vector>
#include <alsa/asoundlib.h>
#include <cstdio>
#include <inttypes.h>
#include "qmxdetector.h"
#include "exception.h"
#include "uevent.h"
#include "dir.h"

static const uint16_t QMX_VID = 0x0483;
static const uint16_t QMX_PID = 0xa34c;

std::string detectQMXAlsaDev()
{
	char **hints;
	const int err(snd_device_name_hint(-1, "pcm", (void ***) &hints));
	xassert(err == 0, "snd_device_name_hint(): %d", err);

	std::string rs;
	for(char **hint(hints); *hint; ++hint) {
		char *ioid(snd_device_name_get_hint(*hint, "IOID"));
		if(ioid) {
			const std::string sioid(ioid);
			free(ioid);

			if(sioid != "Output") {
				continue;
			}
		}

		char *name(snd_device_name_get_hint(*hint, "NAME"));
		if(name) {
			const std::string sname(name);
			free(name);

			if(sname.rfind("hw:CARD=Transceiver,", 0) == 0) {
				rs = sname;
				break;
			}
		}
	}

	snd_device_name_free_hint((void **) hints);
	xassert(!rs.empty(), "QMX ALSA device not found");
	return rs;
}

std::string detectQMXPort()
{
	Dir d("/sys/class/tty");
	std::string ent;
	while(!(ent = d()).empty()) {
		if(ent == "." || ent == "..") {
			continue;
		}

		const UEvent ue(util::format("/sys/class/tty/%s/device/uevent", ent.c_str()));
		if(ue.isValid() && ue.getDriver() == "cdc_acm" && ue.getVID() == QMX_VID && ue.getPID() == QMX_PID) {
			return util::format("/dev/%s", ent.c_str());
		}
	}

	xthrow("QMX serial device not found");
	/* NOTREACHED */
	return "";
}
