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

void pass_signal(int sig) {
    if(child_pid > 0)
        kill(child_pid, sig);
    else {
        fprintf(stderr, "Exiting with signal %i", sig);
        exit(sig);
    }
}

int main(int argc, char *argv[])
{
    if(argc == 1) {
        printf("Starting NexusCoordinator ... ");
        fflush(stdout);

        child_pid = fork();
        if (child_pid == 0)
        {
            execl("/usr/bin/nc-term", "nc-term", "--sh", 0);
            _exit(-1);
        } else if(child_pid > 0) {
            sleep(2);

            int status;
            if(waitpid(child_pid, &status, WNOHANG) == 0) {
                signal(SIGHUP, pass_signal);
                signal(SIGSEGV, pass_signal);
                signal(SIGABRT, pass_signal);
                signal(SIGQUIT, pass_signal);
                signal(SIGTERM, pass_signal);
                signal(SIGKILL, pass_signal);
                signal(SIGTSTP, SIG_IGN);
                signal(SIGINT, SIG_IGN);

                atexit(kill_child);
                for(;;) {
                    waitpid(child_pid, &status, WUNTRACED);

                    if(WIFSTOPPED(status))
                        kill(child_pid, SIGCONT);
                    else
                        return WEXITSTATUS(status);
                }
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
