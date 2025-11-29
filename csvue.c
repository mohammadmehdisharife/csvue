#include <stdio.h>
#include <string.h>

void print_spaces(int count) {
    for (int i = 0; i < count; i++) {
        printf(" ");
    }
}

int main(int argc, char *argv[])
{
  if (argc != 2) {
    fprintf(stderr, "using: %s <filename>\n",argv[0]);
  }

  FILE *file;
  file = fopen(argv[1],"r");
  if (file == NULL) {
    fprintf(stderr, "Failed to open file %s.\n",argv[1]);
    return 1;
  }

  char *cell;
  char buffer[1024];
  int first_line = 1;
  int col_count = 1;

  while ((fgets(buffer, sizeof(buffer), file)) != NULL) {

    cell = strtok(buffer,",");
    int total_spaces = 16 - strlen(cell);
    int left_spaces = total_spaces / 2;
    int right_spaces = total_spaces - left_spaces;

    print_spaces(left_spaces);
    printf("%s", cell);
    print_spaces(right_spaces);

    while((cell = strtok(NULL,",")) != NULL) {

      int total_spaces = 16 - strlen(cell);
      int left_spaces = total_spaces / 2;
      int right_spaces = total_spaces - left_spaces;

      print_spaces(left_spaces);
      printf("%s", cell);
      print_spaces(right_spaces);

      col_count = col_count + 1;
    }

    if (first_line == 1) {
      for (int i = 0; i < col_count; i++) {
        printf(" -------------- ");
      }
      printf("\n");
      first_line = 0;
    }
  }

  printf("\n");
  fclose(file);
  return 0;
}
