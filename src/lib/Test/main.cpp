#include <string.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    char s[] = "\\bin\\ls    -l   foo bar   ";
    char *token, *save_ptr;
    for (token = strtok_r(s, " ", &save_ptr); token != NULL; token = strtok_r(NULL, " ", &save_ptr)) {
        printf ("%s\n", token);
    }


    char **arg[256];
    printf("%d\n", (int)sizeof(char**));

    return 0;
}


















