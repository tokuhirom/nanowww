#include "tap.h"
#include "../nanowww.h"

int main() {
    nanowww::client client;
    nanowww::request req("GET", "http://livedoor.com/", "");
    ok(req.get_uri()->get_host() == std::string("livedoor.com"), "");
    nanowww::response res;
    client.send_request(req, &res);
    // ok(res.get_content(), 0);

    ok(true, "OK");
    done_testing();
}

