#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <ctype.h>

#define DELIMITER ","
#define MAX_COLUMNS 100
#define MAX_ROWS 1000

int get_terminal_width() {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    return w.ws_col;
}

void trim_string(char *str) {
    if (str == NULL) return;
    
    int len = strlen(str);
    while (len > 0 && (isspace(str[len-1]) || str[len-1] == '"')) {
        str[--len] = '\0';
    }
    
    char *start = str;
    while (*start && (isspace(*start) || *start == '"')) {
        start++;
    }
    
    if (start != str) {
        memmove(str, start, strlen(start) + 1);
    }
}

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

    char *data[MAX_ROWS][MAX_COLUMNS];
    int row_count = 0;
    int col_count = 0;
    
    char buffer[1024];
    while (row_count < MAX_ROWS && fgets(buffer, sizeof(buffer), file)) {
        buffer[strcspn(buffer, "\n")] = '\0';
        
        int current_col = 0;
        char *line_ptr = buffer;
        char *token;
        
        int in_quotes = 0;
        char *start = line_ptr;
        
        for (char *p = line_ptr; *p; p++) {
            if (*p == '"') {
                in_quotes = !in_quotes;
            }
            else if (*p == ',' && !in_quotes) {
                *p = '\0';
                token = start;
                trim_string(token);
                
                if (current_col < MAX_COLUMNS) {
                    data[row_count][current_col] = strdup(token);
                    current_col++;
                }
                
                start = p + 1;
            }
        }
        
        token = start;
        trim_string(token);
        if (current_col < MAX_COLUMNS) {
            data[row_count][current_col] = strdup(token);
            current_col++;
        }
        
        if (current_col > col_count) {
            col_count = current_col;
        }
        
        row_count++;
    }
    
    fclose(file);
    
    int col_widths[MAX_COLUMNS] = {0};
    for (int col = 0; col < col_count; col++) {
        for (int row = 0; row < row_count; row++) {
            if (data[row][col] != NULL) {
                int len = strlen(data[row][col]);
                if (len > col_widths[col]) {
                    col_widths[col] = len;
                }
            }
        }
        if (col_widths[col] < 3) col_widths[col] = 3;
    }
    
    int total_width = 0;
    for (int col = 0; col < col_count; col++) {
        total_width += col_widths[col] + 3;
    }
    total_width += 1;
    
    int terminal_width = get_terminal_width();
    if (total_width > terminal_width && col_count > 1) {
        int excess = total_width - terminal_width;
        int per_col_reduction = excess / col_count + 1;
        
        for (int col = 0; col < col_count; col++) {
            col_widths[col] = col_widths[col] > per_col_reduction + 3 ? 
                             col_widths[col] - per_col_reduction : 3;
        }
    }
    
    for (int row = 0; row < row_count; row++) {
        for (int col = 0; col < col_count; col++) {
            if (data[row][col] != NULL) {
                printf("%-*s", col_widths[col], data[row][col]);
            } else {
                printf("%-*s", col_widths[col], "");
            }
            
            if (col < col_count - 1) {
                printf(" │ ");
            }
        }
        printf("\n");
        
        if (row == 0) {
            for (int col = 0; col < col_count; col++) {
                for (int i = 0; i < col_widths[col]; i++) {
                    printf("─");
                }
                if (col < col_count - 1) {
                    printf("─┼─");
                }
            }
            printf("\n");
        }
    }
    
    for (int row = 0; row < row_count; row++) {
        for (int col = 0; col < col_count; col++) {
            if (data[row][col] != NULL) {
                free(data[row][col]);
            }
        }
    }
    
    return 0;
}
