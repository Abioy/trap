#include "asm/inline_hook.h"
#include <common/log.h>

int (*oldStrcmp)(const char *, const char *);

int myStrcmp(const char *s1, const char *s2) {
    LOG_INFO("hooked");

    if (oldStrcmp) {
        LOG_INFO("real result: %d", oldStrcmp(s1, s2));
    }

    return 0;
}

int main() {
    const char *s1 = "hello";
    const char *s2 = "world";

    gInlineHook->hook((void *)strcmp, (void *)myStrcmp, (void **)&oldStrcmp);

    if (strcmp(s1, s2) == 0) {
        LOG_INFO("equal");
    } else {
        LOG_INFO("not equal");
    }

    gInlineHook->unhook((void *)strcmp, (void *)oldStrcmp);

    if (strcmp(s1, s2) == 0) {
        LOG_INFO("equal");
    } else {
        LOG_INFO("not equal");
    }

    return 0;
}
