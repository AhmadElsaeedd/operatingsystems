#include <stdio.h>

int main(int argc, char *argv[]) {
    //Check that argument count is 2 (file name and the argument)
    if(argc != 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }


    //Open the file
    FILE *file = fopen(argv[1], "r");
    if(file == NULL) {
        printf("Failed to open file %s\n", argv[1]);
        return 1;
    }

    char ch;
    while((ch = fgetc(file)) != EOF) {
        putchar(ch);
    }

    fclose(file);
    return 0;
}