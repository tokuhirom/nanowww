#include "../nanowww.h"
#include <nanotap/nanotap.h>
#include "test_util.h"

int main(int argc, char **argv) {
    ignore_sigpipe();

    std::string uri = gen_uri(argc, argv);
    nanowww::Client client;
    client.set_timeout(1);

    nanowww::Request req("GET", uri.c_str());
    req.headers()->set_authorization_basic("dankogai", "kogaidan");

    nanowww::Response res;
    bool ret = client.send_request(req, &res);
    if (!client.errstr().empty()) {
        diag(client.errstr().c_str());
    }
    assert(ret);
    printf("%s\n", res.content().c_str());
    return 0;
}

