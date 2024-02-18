#include<stdio.h>
#include<unistd.h>
int main()
{
    printf("Before execl\n");
    execl("/bin/ps","ps","-a",NULL);//
    printf("After execlp\n");
}