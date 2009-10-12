#include <stdio.h>
#include <assert.h>
#include "../nanowww.h"
#include "../t/tap.h"
#include "../t/test_util.h"
#include <stdio.h>
#include <iostream>
#include <cassert>

int main(int argc, char **argv) {
    ignore_sigpipe();
    std::string uri = gen_uri(argc, argv);

    nanowww::RequestFormData req("POST", uri.c_str());
    req.add_string("hoge", "fuga");
    req.add_string("poke", "take");
    req.add_file("upload", "t/dat/d1.dat");
    req.add_file("nanowww", "nanowww.h");

    nanowww::Response res;
    nanowww::Client client;
    if (client.send_request(req, &res) && "valid response") {
        printf("%s\n", res.content().c_str());
    } else {
        printf("fail\n");
    }
    return 0;
}

