#include "../nanowww.h"
#include "test_util.h"
#include <nanotap/nanotap.h>

int main(int argc, char **argv) {
    ignore_sigpipe();
    std::string uri = gen_uri(argc, argv);

    nanowww::Client client;
    nanowww::Response res;
    bool ret = client.send_get(&res, uri.c_str());
    if (!client.errstr().empty()) {
        diag(client.errstr().c_str());
    }
    assert(ret);

    printf("%s\n", res.content().c_str());

    return 0;
}

