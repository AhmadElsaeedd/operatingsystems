#include <stdio.h>

#define N 4  // Number of processes
#define QUANTUM 4  // Time quantum for the Round Robin scheduling

typedef struct {
    int process_id;
    int arrival_time;
    int burst_time;
    int remaining_time;  // To keep track of the remaining burst time
    int waiting_time;
    int turnaround_time;
    int completion_time;
} Process;

void calculateRoundRobinTimes(Process processes[], int quantum) {
    int current_time = 0;
    int processes_left = N;  // Keep track of the number of processes left to execute

    // Initialize remaining times
    for (int i = 0; i < N; i++) {
        processes[i].remaining_time = processes[i].burst_time;
    }

    while (processes_left > 0) {
        for (int i = 0; i < N; i++) {
            if (processes[i].arrival_time <= current_time && processes[i].remaining_time > 0) {
                if (processes[i].remaining_time > quantum) {
                    current_time += quantum;
                    processes[i].remaining_time -= quantum;
                } else {
                    current_time += processes[i].remaining_time;
                    processes[i].completion_time = current_time;
                    processes[i].turnaround_time = processes[i].completion_time - processes[i].arrival_time;
                    processes[i].waiting_time = processes[i].turnaround_time - processes[i].burst_time;
                    processes[i].remaining_time = 0;
                    processes_left--;
                }
            }
        }
    }

    // Print process stats
    for (int i = 0; i < N; i++) {
        printf("Process P%d - Waiting Time: %d, Turnaround Time: %d, Completion Time: %d\n",
               processes[i].process_id, processes[i].waiting_time, processes[i].turnaround_time, processes[i].completion_time);
    }
}

int main() {
    Process processes[N] = {
        {1, 0, 8},  // Process ID, Arrival Time, Burst Time
        {2, 2, 5},
        {3, 4, 6},
        {4, 6, 7}
    };

    calculateRoundRobinTimes(processes, QUANTUM);

    return 0;
}

/*
Process P1 - Waiting Time: 12, Turnaround Time: 20, Completion Time: 20
Process P2 - Waiting Time: 14, Turnaround Time: 19, Completion Time: 21
Process P3 - Waiting Time: 13, Turnaround Time: 19, Completion Time: 23
Process P4 - Waiting Time: 13, Turnaround Time: 20, Completion Time: 26
*/