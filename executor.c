#include "executor.h"

/**
 * Execute a command using the fork-exec-wait pattern
 *
 * This function demonstrates the fundamental process management pattern
 * used by all Unix shells:
 * 1. Fork a child process
 * 2. Child calls exec to transform into the target command
 * 3. Parent waits for child to complete
 *
 * @param command The command to execute (e.g., "ls", "pwd", "echo")
 * @param args Array of arguments: [command, arg1, arg2, ..., NULL]
 *             Example: {"ls", "-l", NULL} or {"echo", "Hello", NULL}
 * @return Exit status of the command (0=success, non-zero=failure, -1=error)
 */
int execute_command(char *command, char **args) {
    pid_t pid;
    int status;
    /* Create a new process */
    pid = fork();

    if (pid < 0) {
        /* fork failed: check for system resource issues */
        perror("System Error: fork failed");
        return -1;
    }

    if (pid == 0) {
        /* CHILD PROCESS CONTEXT:
         * Attempt to transform this process into the target command.
         * execvp() searches the PATH for the executable.
         */
        if (execvp(command, args) == -1) {
            /* If execvp returns, the command could not be executed */
            perror("Execution Error");
            
            /* Critical: Exit with failure status. We use exit() because 
             * return would send the child back into the main shell loop. */
            exit(1);
        }
    } else {
        /* PARENT PROCESS CONTEXT:
         * Block execution until the specific child process finishes.
         */
        if (waitpid(pid, &status, 0) == -1) {
            perror("System Error: waitpid failed");
            return -1;
        }

        /* Verify that the child exited normally and return its exit code */
        if (WIFEXITED(status)) {
            return WEXITSTATUS(status);
        }
    }

    /* Return -1 if child terminated abnormally (e.g., crashed) */
    return -1;
}