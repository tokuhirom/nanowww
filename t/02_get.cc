#include "tap.h"
#include "../nanowww.h"

int main() {
    nanowww::client client;
    client.get("http://livedoor.com/");

    ok(true);
    done_testing();
}

