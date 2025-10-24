#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    // amount of child-processes
    int n;

    // convert argument
    if (argc > 1) {
        // ASCII to Integer
        n = atoi(argv[1]);
        if (n <= 0) {
            printf("Please provide a positive number!\n");
            exit(1);
        }
    } else {
        printf("Usage: %s <number_of_processes>\n", argv[0]);
        exit(1);
    }

    // Create n child processes
    for (int i = 1; i <= n; i++) {
        pid_t pid = fork();
        
        if (pid == 0) {
            // Child process
            printf("This is child process %d.\n", i);
            
            // Child sleeps for 5 seconds
            sleep(5);
            exit(0);
            
        } else if (pid < 0) {
            // Fork failed
            printf("Fork failed.\n");
            exit(1);
        }
        // Parent continues to create more children
    }

    // Parent process: wait for all children to finish
    for (int i = 0; i < n; i++) {
        wait(NULL);
    }
    
    printf("Parent process finished.\n");
    exit(0);
}