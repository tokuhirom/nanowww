#include <stdio.h>
#include <assert.h>
#include "../nanowww.h"
#include <nanotap/nanotap.h>
#include "../t/test_util.h"
#include <stdio.h>
#include <iostream>
#include <cassert>

int main(int argc, char **argv) {
    ignore_sigpipe();

    nanosocket::SSLSocket::GlobalInit();

    nanowww::Response res;
    nanowww::Client client;
    if (client.send_get(&res, "https://wassr.jp/contact/us") && "valid response") {
        printf("%s\n", res.content().c_str());
    } else {
        printf("%s\n", client.errstr().c_str());
        printf("fail\n");
    }
    return 0;
}

