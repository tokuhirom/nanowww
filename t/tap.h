#ifndef TAP_H
#define TAP_H

#include <stdio.h>
#include <string.h>

static int TEST_COUNT = 0;

static void ok(int x, const char *msg) {
    printf("%s %d - %s\n", (x ? "ok" : "not ok"), ++TEST_COUNT, msg ? msg : "");
}

static void diag(const char *msg) {
    printf("# %s\n", msg ? msg : "");
}

static void string_contains(const char *got, const char *expected, const char *msg) {
    ok(strstr(got, expected) != NULL, msg);
}

static void done_testing() {
    printf("1..%d\n", TEST_COUNT);
}

#ifdef __cplusplus
#include <string>
static void diag(const std::string &msg) {
    diag(msg.c_str());
}

static void string_contains(const std::string &got, const char *expected, const char *msg) {
    string_contains(got.c_str(), expected, msg);
}
#endif

#endif /* TAP_H */
