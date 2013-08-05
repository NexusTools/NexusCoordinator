#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    int fork_rv = fork();
    if (fork_rv == 0)
    {
        execl("/usr/bin/NexusCoordinatorConsole", "NexusCoordinatorConsole", 0);
        _exit(-1);
    } else if(fork_rv > 0) {
        sleep(2);

        int status;
        if(waitpid(fork_rv, &status, WNOHANG) == 0) {
            while (-1 == waitpid(fork_rv, &status, 0));
            return status;
        }
    }

    execl("/bin/bash", "bash", 0);
    execl("/bin/usr/bash", "bash", 0);
    execl("/bin/sh", "sh", 0);
    execl("/bin/usr/sh", "sh", 0);
}
