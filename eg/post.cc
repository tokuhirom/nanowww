#include <stdio.h>
#include <assert.h>
#include "../nanowww.h"
#include "../t/tap.h"
#include <stdio.h>

static void nop_sighandler(int signum) { signum = signum; }

int main(int argc, char **argv) {
    assert(argc == 2);

    {
        struct sigaction sa;
        sa.sa_handler = nop_sighandler;
        sa.sa_flags   = SA_RESTART;
        sigaction(SIGPIPE, &sa, NULL);
    }

    std::string uri = "http://127.0.0.1:";
    uri += argv[1]; // port
    uri += "/";

    std::map<std::string, std::string> data;
    data["hoge"] = "fuga";
    data["page"] = "fuga\x0a";
    data["moge"] = "moge\x3a\xa2";

    nanowww::Response res;
    nanowww::Client client;
    ok(client.send_post(&res, uri.c_str(), data), "response");
    printf("%s\n", res.content().c_str());
    return 0;
}

