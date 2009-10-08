#include "tap.h"
#include "../nanowww.h"

int main() {
    nanowww::Client client;
    nanowww::Request req("GET", "http://livedoor.com/", "");
    ok(req.uri()->host() == std::string("livedoor.com"), "");
    nanowww::Response res;
    int ret = client.send_request(req, &res);
    ok(ret == 1, "send request");
    if (!client.errstr().empty()) {
        diag(client.errstr().c_str());
    }
    ok(res.status() == 200, "status");
    // ok(res.get_content(), 0);

    ok(true, "OK");
    done_testing();
}

