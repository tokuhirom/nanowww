#include "../../nanowww.h"
#include <iostream>

void bench_nanowww(const char *url) {
    nanowww::Client www;
    nanowww::Response res;
    assert(www.send_get(&res, url));
}

int main(int argc, char **argv) {
    assert(argc == 2 && "Usage: a.out port");
    const int N = 10000;

    const char *url = argv[1];

    for (int i=0; i<N; i++) {
        bench_nanowww(url);
    }
}

