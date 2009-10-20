#include "../nanowww.h"
#include <nanotap/nanotap.h>
#include "test_util.h"

int main() {
    {
        nanowww::Headers hdr;
        hdr.set_user_agent("foo");
        is(hdr.get_header("User-Agent"), std::string("foo"));
    }

    {
        nanowww::Request req("GET", "http://example.com/");
        req.set_user_agent("bar");
        is(req.get_header("User-Agent"), std::string("bar"));
    }

    done_testing();
}

