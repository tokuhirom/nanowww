#ifndef TAP_H
#define TAP_H

#include <stdio.h>

int TEST_COUNT = 0;

static inline void ok(bool x, const char *msg) {
    printf("%s %d - %s\n", (x ? "ok" : "not ok"), ++TEST_COUNT, msg ? msg : "");
}

static inline void ok(bool x) {
    ok(x, "");
}

static inline void done_testing() {
    printf("1..%d\n", TEST_COUNT);
}

#endif // TAP_H
