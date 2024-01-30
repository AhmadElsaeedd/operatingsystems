#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[]) {
    if(argc != 2) {
        printf("Usage: %s <N>\n", argv[0]);
        return 1;
    }

    int N = atoi(argv[1]);
    if(N <= 0) {
        printf("N must be a positive integer\n");
        return 1;
    }

    srand(time(NULL)); // Seed the random number generator with the current time

    for(int i = 0; i < N; i++) {
        printf("%d ", rand());
    }

    printf("\n");

    return 0;
}