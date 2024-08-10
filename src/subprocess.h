#ifndef SUBPROCESS_H_
#define SUBPROCESS_H_
#include <stdbool.h>
#include <stddef.h>

#include <unistd.h>

typedef struct {
    int pid;

    int in_pipe[2];
    int out_pipe[2];
}Process;

bool subprocess_run(const char* file, char* const* argv, size_t argv_count);

pid_t subprocess_create(const char* file, char* const* argv, size_t argv_count);
Process subprocess_create_ex(const char* file, char* const* argv, size_t argv_count);

#endif // SUBPROCESS_H_

#ifdef SUBPROCESS_IMPLEMENTATION
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#include <sys/wait.h>
#include <unistd.h>

bool subprocess_run(const char* file, char* const* argv, size_t argv_count) {
    pid_t pid = subprocess_create(file, argv, argv_count);

    for (;;) {
        int pid_status = 0;
        if (waitpid(pid, &pid_status, 0) < 0) {
            fprintf(stderr, "Waitpid failed on %d: %s\n", pid, strerror(errno));
            return false;
        }

        if (WIFEXITED(pid_status)) {
            int exit_status = WEXITSTATUS(pid_status);
            if (exit_status) {
                fprintf(stderr, "Command exited with exit code %d", exit_status);
                return false;
            }

            break;
        }

        if (WIFSIGNALED(pid_status)) {
            fprintf(stderr, "Command process was terminated by %s", strsignal(WTERMSIG(pid_status)));
            return false;
        }
    }

    return true;
}

pid_t subprocess_create(const char* file, char* const* argv, size_t argv_count) {
    pid_t pid = fork();

    char** argv_copy = malloc(sizeof(char*) * (argv_count + 1));
    assert(argv_copy != NULL);
    memcpy(argv_copy, argv, sizeof(char*) * argv_count);
    argv_copy[argv_count] = NULL;

    if (pid < 0) {
        fprintf(stderr, "Couldn't fork: %s\n", strerror(errno));
        free(argv_copy);
        return -1;
    } else if (pid == 0) {
        execvp(file, argv_copy);

        fprintf(stderr, "Couldn't exec %s: %s\n", file, strerror(errno));
        exit(1);
    }

    free(argv_copy);
    return pid;
}

Process subprocess_create_ex(const char* file, char* const* argv, size_t argv_count) {
    Process proc = {};

    proc.pid = fork();
    if (pipe(proc.in_pipe) < 0) {
        fprintf(stderr, "Couldn't create pipe: %s\n", strerror(errno));
        return proc;
    }

    if (pipe(proc.out_pipe) < 0) {
        fprintf(stderr, "Couldn't create pipe: %s\n", strerror(errno));
        return proc;
    }

    char** argv_copy = malloc(sizeof(char*) * (argv_count + 1));
    assert(argv_copy != NULL);
    memcpy(argv_copy, argv, sizeof(char*) * argv_count);
    argv_copy[argv_count] = NULL;

    if (proc.pid < 0) {
        fprintf(stderr, "Couldn't fork: %s\n", strerror(errno));
        free(argv_copy);
        return proc;
    } else if (proc.pid == 0) {
        dup2(proc.in_pipe[0], 0);
        dup2(proc.out_pipe[1], 1);
        dup2(proc.out_pipe[1], 2);

        execvp(file, argv_copy);

        fprintf(stderr, "Couldn't exec %s: %s\n", file, strerror(errno));
        exit(1);
    }

    free(argv_copy);

    return proc;
}

#endif // SUBPROCESS_IMPLEMENTATION