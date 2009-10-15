#include <stdio.h>
#include <assert.h>
#include "../nanowww.h"
#include <nanotap/nanotap.h>
#include "../t/test_util.h"
#include <stdio.h>
#include <iostream>
#include <cassert>

int main() {
    ignore_sigpipe();

    nanosocket::SSLSocket::GlobalInit();

    nanowww::Response res;
    nanowww::Client client;
    if (client.send_get(&res, "https://wassr.jp/contact/us") && "valid response") {
        contains_string(res.content(), "wassr.com", "access to ssl site");
    } else {
        diag(client.errstr().c_str());
        ok(0, "fail");
    }
    done_testing();
}

