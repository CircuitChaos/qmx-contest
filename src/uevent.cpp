#include <cstring>
#include <cstdlib>
#include "uevent.h"
#include "file.h"
#include "util.h"

UEvent::UEvent(const std::string &path)
{
	File fp(fopen(path.c_str(), "rb"));
	if(!fp) {
		return;
	}

	bool gotDriver(false), gotIDs(false);
	while(!gotDriver || !gotIDs) {
		char buf[1024];
		fgets(buf, sizeof(buf), fp);
		if(feof(fp)) {
			break;
		}

		buf[strcspn(buf, "\n")] = 0;
		std::vector<std::string> tok(util::tokenize(buf, "=", 2));
		if(tok.size() != 2) {
			continue;
		}

		if(tok[0] == "DRIVER") {
			driver    = tok[1];
			gotDriver = true;
		}
		else if(tok[0] == "PRODUCT") {
			std::vector<std::string> tok2(util::tokenize(tok[1], "/", 3));
			if(tok2.size() < 2) {
				continue;
			}

			vid    = strtol(tok2[0].c_str(), NULL, 16);
			pid    = strtol(tok2[1].c_str(), NULL, 16);
			gotIDs = true;
		}
	}

	if(gotDriver && gotIDs) {
		valid = true;
	}
}
