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
    ok(res.status() == 301, "status");
    ok(res.message() == std::string("Moved Permanently"), "message");
    ok(res.headers()->get_header("Content-Type") == std::string("text/html; charset=iso-8859-1"), "Content-Type");
    string_contains(res.content(), "<p>The document has moved <a href=\"http://www.livedoor.com/\">here</a>.</p>", "content");

    ok(true, "OK");
    done_testing();
}

