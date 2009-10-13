#include "../nanowww.h"
#include <nanotap/nanotap.h>

int main() {
    nanowww::Headers headers;

    headers.push_header("A", "1");
    headers.push_header("A", "2");
    headers.push_header("B", "3");
    headers.remove_header("B");
    headers.push_header("C", "4");
    headers.set_header("C", "5");

    is(headers.as_string(),
        "A: 1\r\n"
        "A: 2\r\n"
        "C: 5\r\n"
    );

    done_testing();
}

