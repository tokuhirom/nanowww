#include <stdio.h>
#include <assert.h>
#include "../nanowww.h"
#include "tap.h"

int main(int argc, char **argv) {
    assert(argc == 2);

    std::string uri("http://127.0.0.1:");
                uri += argv[1];

    nanowww::Client www;
    nanowww::Response res;
    www.set_timeout(1);
    is(www.send_get(&res, uri), 0, "timeout");
    is(www.errstr(), std::string("Interrupted system call"), "timeout");
    done_testing();
}

