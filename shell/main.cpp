#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    if(argc == 1) {
        printf("Starting NexusCoordinator ... ");
        fflush(stdout);

        int fork_rv = fork();
        if (fork_rv == 0)
        {
            execl("/usr/bin/NexusCoordinatorConsole", "NexusCoordinatorConsole", "--shell", 0);
            _exit(-1);
        } else if(fork_rv > 0) {
            sleep(2);

            int status;
            if(waitpid(fork_rv, &status, WNOHANG) == 0) {
                while (-1 == waitpid(fork_rv, &status, 0));
                return status;
            }
        }

        printf("failed to start.\nDropping to shell...\n\n");
        fflush(stdout);
    }


    strcpy(argv[0], "bash");
    execv("/bin/bash", argv);
    execv("/bin/usr/bash", argv);

    strcpy(argv[0], "sh");
    execv("/bin/sh", argv);
    execv("/bin/usr/sh", argv);

    printf("Neither bash or sh found, exiting...");

    return 1;
}
