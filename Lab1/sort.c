#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Comparison function for qsort
int compare(const void *a, const void *b) {
    return (*(int*)a - *(int*)b);
}

// Reverse comparison function for qsort
int reverse_compare(const void *a, const void *b) {
    return (*(int*)b - *(int*)a);
}

int main(int argc, char *argv[]) {
    FILE *input = stdin;
    FILE *output = stdout;
    int reverse = 0;

    // Parse command-line arguments
    for(int i = 1; i < argc; i++) {
        if(strcmp(argv[i], "-r") == 0) {
            reverse = 1;
        } else if(strcmp(argv[i], "-o") == 0) {
            if(i + 1 < argc) {
                output = fopen(argv[++i], "w");
                if(output == NULL) {
                    printf("Failed to open output file %s\n", argv[i]);
                    return 1;
                }
            } else {
                printf("No output file specified\n");
                return 1;
            }
        } else {
            input = fopen(argv[i], "r");
            if(input == NULL) {
                printf("Failed to open input file %s\n", argv[i]);
                return 1;
            }
        }
    }

    // Read integers from input
    int capacity = 10;
    int size = 0;
    int *numbers = malloc(capacity * sizeof(int));
    while(fscanf(input, "%d", &numbers[size]) == 1) {
        if(++size == capacity) {
            capacity *= 2;
            numbers = realloc(numbers, capacity * sizeof(int));
        }
    }

    // Sort numbers
    qsort(numbers, size, sizeof(int), reverse ? reverse_compare : compare);

    // Write numbers to output
    for(int i = 0; i < size; i++) {
        fprintf(output, "%d\n", numbers[i]);
    }

    // Clean up
    free(numbers);
    if(input != stdin) fclose(input);
    if(output != stdout) fclose(output);

    return 0;
}