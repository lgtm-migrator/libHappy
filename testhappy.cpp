// (C) 2021 by folkert van heusden <mail@vanheusden.com>, released under Apache License v2.0
#include <unistd.h>

#include "sip.h"


// invoked when a new session has started
// one can set 'session->private_data' to point to internal
// data of the callback. you need to free it yourself in
// e.g. the end_session callback.
bool cb_new_session(sip_session_t *const session)
{
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
	generate_beep(440, 0.02, session->samplerate, samples, n_samples);

	return true;
}

// called when we receive a 'BYE' from the peer (and
// the session thus ends)
void cb_end_session(sip_session_t *const session)
{
}

int main(int argc, char *argv[])
{
	// filename, loglevel for logging to file, level for logging to screen
	// levels: debug, info, warning, ll_error
	setlog("testhappy.log", debug, debug);

	// remote ip (IP address of upstream asterisk server), my extension-number, my password, my ip, my sip port, samplerate-used-by-callbacks, [callbacks...]
	sip s("172.29.0.1", "9999", "1234", "172.29.0.107", 5060, 60, 44100, cb_new_session, cb_recv, cb_send, cb_end_session);

	// do whatever you like here, the sleep-loop is not required as long
	// as the sip-object is left instantiated
	for(;;)
		sleep(1);

	return 0;
}
