#ifndef PROCESS_H_
#define PROCESS_H_
#endif // PROCESS_H_

#include <stdint.h>
#include <stdbool.h>
#include <signal.h>

#ifndef PROCESS_MALLOC
#include <stdlib.h>
#define PROCESS_MALLOC malloc
#endif // PROCESS_MALLOC

#ifndef PROCESS_FREE
#include <stdlib.h>
#define PROCESS_FREE free
#endif // PROCESS_FREE

#ifndef PROCESS_ASSERT
#include <assert.h>
#define PROCESS_ASSERT assert
#endif // PROCESS_ASSERT

#define PIPE_READ 0
#define PIPE_WRITE 1

bool process_close(int fd);
bool process_dup2(int fd1, int fd2);
bool process_pipe(int pipefd[2]);

bool process_wait_for_exit(pid_t pid);
int process_start(const char* args[], size_t args_count);

bool process_run(const char* args[], size_t args_count);

typedef struct {
    int pid;
    int in_pipe[2];
    int out_pipe[2];
}Proc;

#define PROCESS_PIPE_STDIN (1 << 0)
#define PROCESS_PIPE_STDOUT (1 << 1)
#define PROCESS_PIPE_STDERR (1 << 2)
#define PROCESS_PIPE_ALL (PROCESS_PIPE_STDIN | PROCESS_PIPE_STDOUT | PROCESS_PIPE_STDERR)

Proc process_start_ex(const char* args[], size_t args_count, uint8_t pipe_flags);
bool prcess_proc_destroy(Proc* proc);

#ifdef PROCESS_IMPLEMENTATION
#undef PROCESS_IMPLEMENTATION
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>

bool process_close(int fd) {
    if (close(fd) < 0) {
        fprintf(stderr, "Couldn't close %d: %s\n", fd, strerror(errno));
        return false;
    }

    return true;
}

bool process_dup2(int fd1, int fd2) {
    if (dup2(fd1, fd2) < 0) {
        fprintf(stderr, "Couldn't dup2: %s\n", strerror(errno));
        return false;
    }

    return true;
}

bool process_pipe(int pipefd[2]) {
    if (pipe(pipefd) < 0) {
        fprintf(stderr, "Couldn't create pipe: %s\n", strerror(errno));
        return false;
    }

    return true;
}

bool process_wait_for_exit(pid_t pid) {
    while (1) {
        int wstatus = 0;
        if (waitpid(pid, &wstatus, 0) < 0) {
            fprintf(stderr, "Couldn't wait for process: %s\n", strerror(errno));
            return false;
        }

        if (WIFEXITED(wstatus)) {
            int estatus = WEXITSTATUS(wstatus);
            if (estatus) {
                fprintf(stderr, "Process exited with status %d\n", estatus);
                return false;
            }

            return true;
        }

        if (WIFSIGNALED(wstatus)) {
            fprintf(stderr, "Process was terminated by signal %d (%s)\n", WTERMSIG(wstatus), strsignal(WTERMSIG(wstatus)));
            return false;
        }
    }
    assert(0);
}

int process_start(const char* args[], size_t args_count) {
    char** args_copy = PROCESS_MALLOC((args_count + 1) * sizeof(char*));
    PROCESS_ASSERT(args_copy != NULL);

    memcpy(args_copy, args, args_count * sizeof(char*));
    args_copy[args_count] = NULL;

    int pid = fork();
    if (pid < 0) {
        fprintf(stderr, "Couldn't fork: %s\n", strerror(errno));
        PROCESS_FREE(args_copy);

        return -1;
    } else if (pid == 0) {
        execvp(args_copy[0], args_copy);

        fprintf(stderr, "Couldn't exec %s: %s\n", args_copy[0], strerror(errno));
        exit(1);
    }

    PROCESS_FREE(args_copy);
    return pid;
}

bool process_run(const char* args[], size_t args_count) {
    int pid = process_start(args, args_count);
    return process_wait_for_exit(pid);
}

Proc process_start_ex(const char* args[], size_t args_count, uint8_t pipe_flags) {
    Proc proc;
    proc.pid = -1;

    char** args_copy = PROCESS_MALLOC((args_count + 1) * sizeof(char*));
    PROCESS_ASSERT(args_copy != NULL);

    memcpy(args_copy, args, args_count * sizeof(char*));
    args_copy[args_count] = NULL;

    bool should_pipe_in = (pipe_flags & PROCESS_PIPE_STDIN) != 0;
    bool should_pipe_out = (pipe_flags & PROCESS_PIPE_STDOUT) != 0;
    bool should_pipe_errout = (pipe_flags & PROCESS_PIPE_STDERR) != 0;

    if (should_pipe_in) {
        if (!process_pipe(proc.in_pipe)) return proc;
    }

    if (should_pipe_out || should_pipe_errout) {
        if (!process_pipe(proc.out_pipe)) return proc;
    }

    proc.pid = fork();
    if (proc.pid < 0) {
        fprintf(stderr, "Couldn't fork: %s\n", strerror(errno));
        return proc;
    } else if (proc.pid == 0) {
        if (should_pipe_in) {
            if (!process_dup2(proc.in_pipe[PIPE_READ], STDIN_FILENO)) exit(1);
            if (!process_close(proc.in_pipe[PIPE_WRITE])) exit(1);
        }

        if (should_pipe_out) {
            if (!process_dup2(proc.out_pipe[PIPE_WRITE], STDOUT_FILENO)) exit(1);
        }

        if (should_pipe_errout) {
            if (!process_dup2(proc.out_pipe[PIPE_WRITE], STDERR_FILENO)) exit(1);
        }

        if (should_pipe_out || should_pipe_errout) {
            if (!process_close(proc.out_pipe[PIPE_READ])) exit(1);
        }

        execvp(args_copy[0], args_copy);

        fprintf(stderr, "Couldn't exec %s: %s\n", args_copy[0], strerror(errno));
        exit(1);
    }

    if (should_pipe_in) {
        if (!process_close(proc.in_pipe[PIPE_READ])) exit(1);
    }

    if (should_pipe_out || should_pipe_errout) {
        if (!process_close(proc.out_pipe[PIPE_WRITE])) exit(1);
    }

    PROCESS_FREE(args_copy);
    return proc;
}

bool process_proc_destroy(Proc* proc) {
    if (!process_close(proc->in_pipe[PIPE_WRITE])) return false;
    if (!process_close(proc->out_pipe[PIPE_READ])) return false;
    return true;
}

#endif // PROCESS_IMPLEMENTATION
