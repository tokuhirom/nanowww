#ifndef TAP_H
#define TAP_H

#include <stdio.h>

int TEST_COUNT = 0;

static void ok(int x, const char *msg) {
    printf("%s %d - %s\n", (x ? "ok" : "not ok"), ++TEST_COUNT, msg ? msg : "");
}

static void diag(const char *msg) {
    printf("# %s\n", msg ? msg : "");
}

static void done_testing() {
    printf("1..%d\n", TEST_COUNT);
}

#ifdef __cplusplus
#include <string>
static void diag(const std::string &msg) {
    diag(msg.c_str());
}
#endif

#endif /* TAP_H */
