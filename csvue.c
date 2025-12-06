#include <stdio.h>
#include <string.h>
#include "./terminal.h"

#define DELIMITER ","

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "usage: %s <filename>\n", argv[0]);
        return 1;
    }

    FILE *file;
    file = fopen(argv[1], "r");
    if (file == NULL) {
        fprintf(stderr, "open file failed\n");
        return 1;
    }

    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), file)) {
        int token_count = 0;
        char line[1024];
        char *token;

        token = strtok(buffer, DELIMITER);
        if (token == NULL) {
            continue;
        }
        strcpy(line, token);
        strcat(line, ",");
        token_count++;
        
        while ((token = strtok(NULL, DELIMITER)) != NULL) {
            strcat(line, token);
            strcat(line, ",");
            token_count++;
        }

        int width = get_terminal_width();
        int space_width = (width - strlen(line) + token_count) / (token_count + 1);
        char space[1024] = " ";
        for (int count = 1;space_width > count;count++) {
            strcat(space, " ");
        }

        printf("%s%s",space,strtok(line,DELIMITER));
        while ((token = strtok(NULL, DELIMITER)) != NULL) {
            printf("%s%s",space,token);
        }
    }
    return 0;
}
