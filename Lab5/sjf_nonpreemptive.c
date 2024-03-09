#include <stdio.h>
#include <stdbool.h>

typedef struct {
    int process_id;
    int arrival_time;
    int burst_time;
    int waiting_time;
    int turnaround_time;
    int completion_time;
} Process;

// Function to sort processes by arrival time
void sortByArrival(Process processes[], int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (processes[j].arrival_time > processes[j + 1].arrival_time) {
                // swap the processes
                Process temp = processes[j];
                processes[j] = processes[j + 1];
                processes[j + 1] = temp;
            }
        }
    }
}

// Function to find the index of the next process to execute
int findNextProcess(Process processes[], int n, int current_time) {
    int min_index = -1, min_burst_time = 9999;
    for (int i = 0; i < n; i++) {
        if (processes[i].arrival_time <= current_time && processes[i].burst_time < min_burst_time && processes[i].completion_time == 0) {
            min_burst_time = processes[i].burst_time;
            min_index = i;
        }
    }
    return min_index;
}

void calculateTimes(Process processes[], int n) {
    int current_time = 0, total_processes_executed = 0, index;
    float total_waiting_time = 0.0, total_turnaround_time = 0.0;

    // Sort processes by arrival time
    sortByArrival(processes, n);

    while (total_processes_executed < n) {
        index = findNextProcess(processes, n, current_time);
        if (index != -1) { // if a process is found to execute
            current_time += processes[index].burst_time;
            processes[index].completion_time = current_time;
            processes[index].turnaround_time = processes[index].completion_time - processes[index].arrival_time;
            processes[index].waiting_time = processes[index].turnaround_time - processes[index].burst_time;

            total_waiting_time += processes[index].waiting_time;
            total_turnaround_time += processes[index].turnaround_time;

            total_processes_executed++;
        } else { // if no process is ready yet
            current_time++;
        }
    }

    // Calculate and print average waiting time and average turnaround time
    printf("Average Waiting Time: %.2f\n", total_waiting_time / n);
    printf("Average Turnaround Time: %.2f\n", total_turnaround_time / n);
}

int main() {
    Process processes[4] = {
        {1, 0, 6},
        {2, 2, 3},
        {3, 4, 2},
        {4, 6, 8}
    };

    calculateTimes(processes, 4);

    // Display results
    for (int i = 0; i < 4; i++) {
        printf("Process P%d - Waiting Time: %d, Turnaround Time: %d, Completion Time: %d\n",
               processes[i].process_id, processes[i].waiting_time, processes[i].turnaround_time, processes[i].completion_time);
    }

    return 0;
}


/*
Average Waiting Time: 3.25
Average Turnaround Time: 8.00
Process P1 - Waiting Time: 0, Turnaround Time: 6, Completion Time: 6
Process P2 - Waiting Time: 6, Turnaround Time: 9, Completion Time: 11
Process P3 - Waiting Time: 2, Turnaround Time: 4, Completion Time: 8
Process P4 - Waiting Time: 5, Turnaround Time: 13, Completion Time: 19
*/