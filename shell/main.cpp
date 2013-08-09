#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int child_pid = 0;

int main(int argc, char *argv[])
{
    if(argc == 1) {
        printf("Starting NexusCoordinator ... ");
        fflush(stdout);

        int status;
        child_pid = fork();
        if (child_pid == 0) {
            execl("/usr/bin/nc-term", "nc-term", "--test-terminal", 0);
            _exit(-1);
        } else if(child_pid > 0) {
            int tLeft = 20; // Wait 2 seconds for test to complete before assuming it failed
            while(tLeft > 0 && waitpid(child_pid, &status, WNOHANG) == 0) {
                usleep(100000); // Sleep 100ms
                tLeft--;
            }

            kill(child_pid, SIGTERM);
            if(tLeft <= 0 && WEXITSTATUS(status) == 0)
                execl("/usr/bin/nc-term", "nc-term", "--shell", 0);
        } else
            child_pid = 0;

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
