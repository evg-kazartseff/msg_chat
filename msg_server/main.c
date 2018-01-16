#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "server.h"
#include "message_queue.h"

int main() {
    int flg = ServerRun("/dev/", 'A');
    if (flg != 0)
        exit(EXIT_FAILURE);

    while (1) {
        printf("Press \"\\c\" for exit!\n");
        char c[2];
        scanf("%s", c);
        if (strcmp(c, "\\c") == 0) {
            ServerFinalize();
            break;
        }
    }
}