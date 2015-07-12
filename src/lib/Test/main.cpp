#include <string.h>
#include <stdio.h>
#include <cstring>

int main() {
    char file_name[] = "\\bin\\ls    -l   foo bar   ";

    int argc = 0;
    char *argv[256];
    /* Tokenize the command */
    char* next;
    char *arg = strtok_r(file_name, " ", &next);
    while (arg != NULL) {
        argv[argc] = arg;
        ++argc;
        arg = strtok_r(NULL, " ", &next);
    }

    for (int i = argc - 1; i >= 0; --i) {
        printf("%s\n", argv[i]);
    }

    for (int i = argc - 1; i >= 0; --i) {
        printf("0x%x\n", &argv[i]);
    }

    printf("%d\n", (int)sizeof(char*));
    return 0;
}


















