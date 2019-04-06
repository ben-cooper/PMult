#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int fd[2];
    int val = 0;
    int val2 = 0;

    // create pipe descriptors
    pipe(fd);

    // fork() returns 0 for child process, child-pid for parent process.
    if (fork() != 0)
    {
        // parent: writing only, so close read-descriptor.
        close(fd[0]);

        // send the value on the write-descriptor.
        val = 100;
        val2 = 200;
        write(fd[1], &val, sizeof(val));
        write(fd[1], &val2, sizeof(val2));
        printf("Parent(%d) send values: %d %d\n", getpid(), val, val2);

        // close the write descriptor
        close(fd[1]);
    }
    else
    {   // child: reading only, so close the write-descriptor
        close(fd[1]);

        // now read the data (will block)
        read(fd[0], &val, sizeof(val));
        read(fd[0], &val2, sizeof(val2));
        printf("Child(%d) received values: %d %d\n", getpid(), val, val2);

        // close the read-descriptor
        close(fd[0]);
    }
    return 0;
}
