#ifndef TEST_UTIL_H_
#define TEST_UTIL_H_

static inline void nop_sighandler(int signum) { signum = signum; }
static inline void ignore_sigpipe() {
    struct sigaction sa;
    sa . sa_handler = nop_sighandler;
    sa . sa_flags   = SA_RESTART;
    sigaction( SIGPIPE, &sa, NULL );
}

inline std::string gen_uri(int argc, char **argv) {
    assert(argc == 2);
    std::string uri = "http://127.0.0.1:";
    uri += argv[1]; // port
    uri += "/";
    return uri;
}

#endif // TEST_UTIL_H_
