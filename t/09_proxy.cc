#include "../nanowww.h"
#include <nanotap/nanotap.h>
#include "test_util.h"

int main(int argc, char **argv) {
    ignore_sigpipe();
    assert(argc == 3);

    std::string uri       = std::string("http://0.0.0.0:") + argv[1];
    std::string proxy_uri = std::string("http://0.0.0.0:") + argv[2];

    nanowww::Client client;
    client.set_timeout(1);
    client.set_proxy(proxy_uri);
    nanowww::Response res;
    bool ret = client.send_get(&res, uri.c_str());
    if (!client.errstr().empty()) {
        diag(client.errstr().c_str());
    }
    assert(ret);
    printf("%s\n", res.content().c_str());
    return 0;
}

