// see RFC 4648
#include "../nanobase.h"
#include "../extlib/nanotap.h"

#include <stdio.h>
#include <stdlib.h>

#define TEST(x,y) do { \
        char *tmp = (char*)malloc(nb_base64_needed_encoded_length(strlen(x)) * sizeof(char)); \
        nb_base64_encode((const unsigned char*)x, strlen(x), (unsigned char*)tmp); \
        ok(strcmp(tmp, y) == 0, x); \
        free(tmp); \
    } while (0)

int main() {
    ok(nb_base64_needed_encoded_length(10) == 17, "nb_base64_needed_encoded_length");
    TEST( "",       "" );
    TEST( "f",      "Zg==" );
    TEST( "fo",     "Zm8=" );
    TEST( "foo",    "Zm9v" );
    TEST( "foob",   "Zm9vYg==" );
    TEST( "fooba",  "Zm9vYmE=" );
    TEST( "foobar", "Zm9vYmFy" );
    done_testing();
}

