#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
    // Check if the number of command-line arguments is not 3 (program name, 'old' character, 'new' character)
    if(argc != 3) {
        printf("Usage: %s <'old'> <'new'>\n", argv[0]);
        return 1;
    }

    // Remove quotes if present from 'old' and 'new' characters
    char oldChar = argv[1][strlen(argv[1]) > 1 ? 1 : 0];
    char newChar = argv[2][strlen(argv[2]) > 1 ? 1 : 0];

    int ch;
    // Read characters from stdin until EOF is reached
    while((ch = getchar()) != EOF) {
        // If the read character is the 'old' character, write the 'new' character to stdout
        if(ch == oldChar) {
            putchar(newChar);
        } else {
            // If the read character is not the 'old' character, write the read character to stdout
            putchar(ch);
        }
    }

    return 0;
}