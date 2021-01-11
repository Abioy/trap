#include "asm/inline_hook.h"
#include <cstring>

int (*oldStrcmp)(const char *, const char *);

int myStrcmp(const char *s1, const char *s2) {
    printf("hooked\n");

    if (oldStrcmp) {
        printf("real result: %d\n", oldStrcmp(s1, s2));
    }

    return 0;
}

int main() {
    const char *s1 = "hello";
    const char *s2 = "world";

    if (!gInlineHook->hook((void *)strcmp, (void *)myStrcmp, (void **)&oldStrcmp))
        return -1;

    if (strcmp(s1, s2) == 0) {
        printf("equal\n");
    } else {
        printf("not equal\n");
    }

    if (!gInlineHook->unhook((void *)strcmp, (void *)oldStrcmp))
        return -1;

    if (strcmp(s1, s2) == 0) {
        printf("equal\n");
    } else {
        printf("not equal\n");
    }

    return 0;
}
