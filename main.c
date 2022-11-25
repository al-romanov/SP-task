#include <signal.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

enum {
    UNTRIGGERED_SLEEP_TIME = 200000,
    TRIGGERED_SLEEP_TIME = 2000000,
    N_ITERATIONS = 7
};
int this_process_turn = 0;
int this_process_iterations = 0;

void trigger_handler(int sig) {
    this_process_turn ^= 1;
    ++this_process_iterations;
    printf("Process %d is waking up in %d time...\n", getpid(), this_process_iterations);
    usleep(TRIGGERED_SLEEP_TIME);
}

void execute_trigger(pid_t companion_pid) {
    while (this_process_turn == 0) {
        usleep(UNTRIGGERED_SLEEP_TIME);
    }
    while (this_process_iterations < N_ITERATIONS) {
        pid_t this_pid = getpid();
        printf("Process %d triggers process %d\n", this_pid, companion_pid);
        this_process_turn = 0;
        kill(companion_pid, SIGUSR1);
        printf("Process %d is sleeping...\n", this_pid);
        while (this_process_turn == 0) {
            usleep(UNTRIGGERED_SLEEP_TIME);
        }
    }
}

int main() {
    void (*prev_handler)(int) = signal(SIGUSR1, trigger_handler);
    pid_t companion_pid = getpid(); // for child_process
    printf("Parent pid is %d\n", companion_pid);
    pid_t child_pid = fork(); // for parent process
    if (child_pid == 0) {
        this_process_turn = 1;
    } else {
        companion_pid = child_pid;
        printf("child pid is %d\n", child_pid);
    }
    execute_trigger(companion_pid);
    if (child_pid != 0) {
        kill(companion_pid, SIGUSR1); // wake up child process
        int status = 0;
        waitpid(child_pid, &status, 0);
    }
    signal(SIGUSR1, prev_handler);
    return 0;
}