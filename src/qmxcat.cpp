#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <termios.h>
#include <unistd.h>
#include <cerrno>
#include <fcntl.h>
#include "exception.h"
#include "qmxcat.h"
#include "util.h"

static const unsigned TIMEOUT = 3;

QMXCat::QMXCat(const std::string &port)
    : fd(open(port.c_str(), O_RDWR | O_NOCTTY))
{
	xassert(fd != -1, "Could not open device %s: %m", port.c_str());

	termios t;
	xassert(tcgetattr(fd, &t) != -1, "Could not get port attributes: %m");

	t.c_iflag = 0;
	t.c_oflag = 0;
	t.c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN | ISIG);
	t.c_cflag &= ~(CSIZE | PARENB);
	t.c_cflag |= CS8;

	t.c_cc[VMIN]  = 1;
	t.c_cc[VTIME] = 0;

	xassert(cfsetispeed(&t, B115200) != -1, "Could not set input speed: %m");
	xassert(cfsetospeed(&t, B115200) != -1, "Could not set output speed: %m");
	xassert(tcsetattr(fd, TCSAFLUSH, &t) != -1, "Could not set port attributes: %m");
	xassert(exchange("ID;", 6) == "ID020;", "Invalid radio ID");
}

QMXCat::~QMXCat()
{
	tcdrain(fd);
}

int QMXCat::getFD() const
{
	return fd;
}

void QMXCat::sendReq(const std::string &s)
{
	// TODO this does not work, do it differently (util::watch())
	char buf[1024];
	while(recv(fd, buf, sizeof(buf), MSG_DONTWAIT) >= 0) {
	}

	// TODO handle partial write, can it happen here?
	const ssize_t rs(write(fd, s.data(), s.size()));
	xassert(rs != 0, "write() returned zero, this shouldn't happen");
	xassert(rs > 0, "Error sending data to QMX: %m");
	xassert(rs == (ssize_t) s.size(), "Tried to send %zu bytes to QMX, but only %zd sent", s.size(), rs);
}

std::string QMXCat::recvRsp(size_t numBytes)
{
	std::string str;
	size_t remaining(numBytes);
	while(remaining) {
		const std::set<int> outfds(util::watch({fd}, TIMEOUT));
		if(!util::inSet<int>(outfds, fd)) {
			xthrow("Timeout reading CAT response from radio");
		}

		std::vector<char> buf;
		buf.resize(remaining);
		const ssize_t readrs(read(fd, &buf[0], remaining));
		xassert(readrs != 0, "EOF when reading CAT response from radio -- radio disconnected?");

		if(readrs < 0) {
			if(errno == EAGAIN || errno == EINTR) {
				// TODO recalculate timeout
				continue;
			}

			xthrow("read(): %m");
		}

		xassert(readrs <= (ssize_t) remaining, "read(): returned %zd, but should %zu max", readrs, remaining);

		/* This is a workaround for a bug in QMX FW 17. AG (gain) response contains
		 * a stray zero. So we want to receive 7 bytes ("AGnnnn;"), but receive 8
		 * ("AGnnnn\0;"). I assume it will be corrected one day, so I just added
		 * this workaround here.
		 */

		// str += std::string(&buf[0], readrs);
		// remaining -= readrs;

		for(size_t i(0); i < (size_t) readrs; ++i) {
			if(!buf[i]) {
				continue;
			}

			str += buf[i];
			remaining--;
		}
	}

	return str;
}

std::string QMXCat::exchange(const std::string &req, size_t numResponseBytes)
{
	sendReq(req);
	return recvRsp(numResponseBytes);
}

unsigned QMXCat::getGain()
{
	const std::string rsp(exchange("AG;", 7));
	xassert(rsp.substr(0, 2) == "AG" && rsp[rsp.size() - 1] == ';', "Invalid response to AG");
	return strtol(rsp.substr(2, 4).c_str(), NULL, 10);
}

void QMXCat::setGain(unsigned gain)
{
	sendReq(util::format("AG%03u;", gain));
}

uint32_t QMXCat::getFreq()
{
	const std::string rsp(exchange("FA;", 14));
	xassert(rsp.substr(0, 2) == "FA" && rsp[rsp.size() - 1] == ';', "Invalid response to FA");
	return strtol(rsp.substr(2, 11).c_str(), NULL, 10);
}

void QMXCat::setFreq(uint32_t freq)
{
	sendReq(util::format("FA%07u;", freq));
}

bool QMXCat::isCW()
{
	const std::string rsp(exchange("MD;", 4));
	if(rsp == "MD3;") {
		return true;
	}

	if(rsp == "MD6;") {
		return false;
	}

	xthrow("Invalid or unsupported response to MD: %s", rsp.c_str());
	/* NOTREACHED */
	return false;
}

void QMXCat::setCW(bool cw)
{
	sendReq(util::format("MD%d;", cw ? 3 : 6));
}

bool QMXCat::isTX()
{
	const std::string rsp(exchange("TQ;", 4));
	if(rsp == "TQ0;") {
		return false;
	}

	if(rsp == "TQ1;") {
		return true;
	}

	xthrow("Invalid or unsupported response to TQ: %s", rsp.c_str());
	/* NOTREACHED */
	return false;
}

void QMXCat::setTX(bool tx)
{
	sendReq(util::format("TQ%d;", tx ? 1 : 0));
}
