#include "tap.h"
#include "../nanowww.h"

int main() {
    nanowww::Client client;
    nanowww::Request req("GET", "http://tinyurl.com/7n29f", "");
    ok(req.uri()->host() == std::string("tinyurl.com"), "");
    nanowww::Response res;
    bool ret = client.send_request(req, &res);
    ok(ret, "send request");
    if (!client.errstr().empty()) {
        diag(client.errstr().c_str());
    }
    is(res.status(), 200, "status");
    is(res.message(), std::string("OK"), "message");
    is(res.headers()->get_header("Content-Type"), std::string("text/html; charset=EUC-JP"), "Content-Type");
    string_contains(res.content(), "www.find-job.net", "content");

    ok(true, "OK");
    done_testing();
    return 0;
}

