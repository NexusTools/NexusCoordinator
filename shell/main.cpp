#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int child_pid = 0;

void kill_child() {
    kill(child_pid, SIGTERM);
    kill(child_pid, SIGKILL);
}

int main(int argc, char *argv[])
{
    if(argc == 1) {
        printf("Starting NexusCoordinator ... ");
        fflush(stdout);

        child_pid = fork();
        if (child_pid == 0)
        {
            execl("/usr/bin/NexusCoordinatorConsole", "NexusCoordinatorConsole", "--shell", 0);
            _exit(-1);
        } else if(child_pid > 0) {
            sleep(2);

            int status;
            if(waitpid(child_pid, &status, WNOHANG) == 0) {
                atexit(kill_child);
                while (-1 == waitpid(child_pid, &status, 0));
                return WEXITSTATUS(status);
            }
            kill_child();
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
