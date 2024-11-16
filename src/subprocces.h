#ifndef SUBPROCCES_H_
#define SUBPROCCES_H_

#include <stddef.h>
#include <stdarg.h>
#include <stdbool.h>
#include <sys/types.h>

typedef struct {
    pid_t pid;
    int pipe_in[2];
    int pipe_out[2];
}Proc;

#define PIPE_READ 0
#define PIPE_WRITE 1

pid_t start_proccesvl(char* file, size_t args_count, va_list args);
pid_t start_proccesv(char* file, ...);
pid_t start_procces(char* file, char** args, size_t args_count);
bool wait_for_process(pid_t pid);

Proc start_procces_ex(char* file, char** args, size_t args_count);
Proc start_procces_exvl(char* file, size_t args_count, va_list args);
Proc start_procces_exv(char* file, ...);

#endif // SUBPROCCES_H_

#ifdef SUBPROCCES_IMPLEMENTATION
#undef SUBPROCCES_IMPLEMENTATION

#include <string.h>
#include <errno.h>

#include <unistd.h>
#include <sys/wait.h>

char* strsignal(int sig);

pid_t start_proccesvl(char* file, size_t args_count, va_list args) {
    char* argv[args_count + 2];
    argv[0] = file;

    for (size_t i = 0; i < args_count; ++i) {
        argv[i + 1] = va_arg(args, char*);
    }

    argv[args_count + 1] = NULL;

    pid_t pid = fork();

    if (pid < 0) {
        fprintf(stderr, "Couldn't fork: %s\n", strerror(errno));
        return -1;
    } else if (pid == 0) {
        execvp(file, argv);

        fprintf(stderr, "Couldn't exec %s: %s\n", file, strerror(errno));
        _exit(1);
    }

    return pid;
}

pid_t start_proccesv(char* file, ...) {
    va_list args;
    va_start(args, file);

    size_t count = 0;

    char* arg = va_arg(args, char*);
    while (arg != NULL) {
        ++count;
        arg = va_arg(args, char*);
    }

    va_end(args);
    va_start(args, file);

    pid_t pid = start_proccesvl(file, count, args);

    va_end(args);
    return pid;
}

pid_t start_procces(char* file, char** args, size_t args_count) {
    char* argv[args_count + 2];
    argv[0] = file;

    for (size_t i = 0; i < args_count; ++i) {
        argv[i] = args[i];
    }

    argv[args_count + 1] = NULL;

    pid_t pid = fork();

    if (pid < 0) {
        fprintf(stderr, "Couldn't fork: %s\n", strerror(errno));
        return -1;
    } else if (pid == 0) {
        execvp(file, argv);

        fprintf(stderr, "Couldn't exec %s: %s\n", file, strerror(errno));
        _exit(1);
    }

    return pid;
}

Proc start_procces_ex(char* file, char** args, size_t args_count) {
    char* argv[args_count + 2];
    argv[0] = file;

    for (size_t i = 0; i < args_count; ++i) {
        argv[i] = args[i];
    }

    argv[args_count + 1] = NULL;

    Proc proc = {0};

    if (pipe(proc.pipe_in) < 0) {
        fprintf(stderr, "Couldn't create pipe: %s\n", strerror(errno));
        return proc;
    }

    if (pipe(proc.pipe_out) < 0) {
        fprintf(stderr, "Couldn't create pipe: %s\n", strerror(errno));
        return proc;
    }

    proc.pid = fork();
    if (proc.pid < 0) {
        fprintf(stderr, "Couldn't fork: %s\n", strerror(errno));
        return proc;
    } else if (proc.pid == 0) {
        dup2(proc.pipe_in[PIPE_READ], 0);
        dup2(proc.pipe_out[PIPE_WRITE], 1);
        dup2(proc.pipe_out[PIPE_WRITE], 2);

        execvp(file, argv);

        fprintf(stderr, "Couldn't exec %s: %s\n", file, strerror(errno));
        _exit(1);
    }

    return proc;
}

Proc start_procces_exvl(char* file, size_t args_count, va_list args) {
    char* argv[args_count + 2];
    argv[0] = file;

    for (size_t i = 0; i < args_count; ++i) {
        argv[i + 1] = va_arg(args, char*);
    }

    argv[args_count + 1] = NULL;

    Proc proc = {0};

    if (pipe(proc.pipe_in) < 0) {
        fprintf(stderr, "Couldn't create pipe: %s\n", strerror(errno));
        return proc;
    }

    if (pipe(proc.pipe_out) < 0) {
        fprintf(stderr, "Couldn't create pipe: %s\n", strerror(errno));
        return proc;
    }

    proc.pid = fork();
    if (proc.pid < 0) {
        fprintf(stderr, "Couldn't fork: %s\n", strerror(errno));
        return proc;
    } else if (proc.pid == 0) {
        dup2(proc.pipe_in[PIPE_READ], 0);
        dup2(proc.pipe_out[PIPE_WRITE], 1);
        dup2(proc.pipe_out[PIPE_WRITE], 2);

        execvp(file, argv);

        fprintf(stderr, "Couldn't exec %s: %s\n", file, strerror(errno));
        _exit(1);
    }

    return proc;
}

Proc start_procces_exv(char* file, ...) {
    va_list args;
    va_start(args, file);

    size_t count = 0;

    char* arg = va_arg(args, char*);
    while (arg != NULL) {
        ++count;
        arg = va_arg(args, char*);
    }

    va_end(args);
    va_start(args, file);

    Proc proc = start_procces_exvl(file, count, args);

    va_end(args);
    return proc;
}


bool wait_for_process(pid_t pid) {
    for (;;) {
        int wstatus;
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

            break;
        }

        if (WIFSIGNALED(wstatus)) {
            fprintf(stderr, "Process was terminated by signal %d (%s)\n", WTERMSIG(wstatus), strsignal(WTERMSIG(wstatus)));
            return false;
        }
    }

    return true;
}

#endif // SUBPROCCES_IMPLEMENTATION
