// (C) 2022 by folkert van heusden <mail@vanheusden.com>, released under Apache License v2.0
#include <unistd.h>

#include "sip.h"


// invoked when a new session has started
// one can set 'session->private_data' to point to internal
// data of the callback. you need to free it yourself in
// e.g. the end_session callback.
bool cb_new_session(sip_session_t *const session, const std::string & from)
{
	printf("cb_new_session, call-id: %s, caller: %s\n", session->call_id.c_str(), from.c_str());

	return true;
}

// invoked when the peer produces audio and which is then
// received by us
bool cb_recv(const short *const samples, const size_t n_samples, sip_session_t *const session)
{
	FILE *fh = fopen("test.pcm", "a+");
	if (fh) {
		fwrite(samples, sizeof(short), n_samples, fh);

		fclose(fh);
	}

	return true;
}

// invoked when the library wants to send audio to
// the peer
bool cb_send(short **const samples, size_t *const n_samples, sip_session_t *const session)
{
	generate_beep(800, 0.04, session->samplerate, samples, n_samples);

	return true;
}

// called when we receive a 'BYE' from the peer (and
// the session thus ends)
void cb_end_session(sip_session_t *const session)
{
}

// invoked when a DTMF signal has been received
// note that may come in multiple times for the
// same key-press. this is due to how they are
// transmitted
void cb_dtmf(const uint8_t dtmf_code, const bool is_end, const uint8_t volume, sip_session_t *const session)
{
	printf("DTMF pressed: %d\n", dtmf_code);
}

int main(int argc, char *argv[])
{
	// filename, loglevel for logging to file, level for logging to screen
	// levels: debug, info, warning, ll_error
	setlog("testhappy.log", debug, debug);

	// remote ip (IP address of upstream asterisk server), my extension-number, my password, my ip, my sip port, samplerate-used-by-callbacks, [callbacks...], pointer to global private data (or nullptr)
	// note: 'my ip' is only required when the library cannot figure out what IP address to use to contact the SIP server. This can happen when there's a NAT router in between for example.
	//sip s("172.29.0.1", "9999", "1234", "172.29.0.107", 5060, 60, 44100, cb_new_session, cb_recv, cb_send, cb_end_session, cb_dtmf, nullptr);
	sip s("192.168.64.204", "9997", "1234", { }, 5060, 60, 44100, cb_new_session, cb_recv, cb_send, cb_end_session, cb_dtmf, nullptr);

	// do whatever you like here, the sleep-loop is not required as long
	// as the sip-object is left instantiated
	for(;;)
		sleep(1);

	return 0;
}
