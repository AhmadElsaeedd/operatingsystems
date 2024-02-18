#include "fcntl.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "sys/wait.h"
#include "unistd.h"

/*A necessary system call for this assignment is the duplicate system call.
There are two varients:
    - dup(int _fd)
    - dup2(int _fd)

The first one receives a file descriptor, duplicates it and returns another 
descriptor on the same file, so for example:
    
    int fd = dup(stdin) OR dup(0) (since stdin is the file descriptor 0)

will duplicate stdin and return another descriptor (that we storein fd)
now, reading from fd is the same as reading from stdin. In fact you can
read from both. Since they both refer to the same file.

Another example is shown below*/

int main(){
    int fd = open("myFile.txt", O_RDONLY);
    int newFD = dup(fd);

    char buffer[128];

    // read 5 bytes from the old file descriptor
    read(fd, buffer, 5);
    printf("%s",buffer);

    // read the rest of the bytes from the new one 
    read(newFD, buffer, 128);
    printf("%s", buffer);

    close(fd);
    close(newFD);
    return 0;
}