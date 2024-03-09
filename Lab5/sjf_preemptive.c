#include <stdio.h>
#include <stdbool.h>

typedef struct {
    int process_id;
    int arrival_time;
    int burst_time;
    int remaining_time;
    int waiting_time;
    int turnaround_time;
    int completion_time;
} Process;

// Function to find the index of the process with the shortest burst time
int findShortestJob(Process processes[], int n, int current_time) {
    int min_time = 9999, index = -1;
    for (int i = 0; i < n; i++) {
        if (processes[i].arrival_time <= current_time && processes[i].remaining_time < min_time && processes[i].remaining_time > 0) {
            min_time = processes[i].remaining_time;
            index = i;
        }
    }
    return index;
}

void calculateTimes(Process processes[], int n) {
    int completed_processes = 0, current_time = 0, shortest_index;
    bool is_completed[n];

    // Initialize remaining times and completion flag
    for (int i = 0; i < n; i++) {
        processes[i].remaining_time = processes[i].burst_time;
        is_completed[i] = false;
    }

    // Iterate until all processes are completed
    while (completed_processes < n) {
        shortest_index = findShortestJob(processes, n, current_time);

        // If no process is ready at the current time
        if (shortest_index == -1) {
            current_time++;
            continue;
        }

        // Process the shortest job
        processes[shortest_index].remaining_time--;

        // If process is completed
        if (processes[shortest_index].remaining_time == 0) {
            completed_processes++;
            processes[shortest_index].completion_time = current_time + 1;
            processes[shortest_index].turnaround_time = processes[shortest_index].completion_time - processes[shortest_index].arrival_time;
            processes[shortest_index].waiting_time = processes[shortest_index].turnaround_time - processes[shortest_index].burst_time;
            is_completed[shortest_index] = true;
        }

        // Increment current time
        current_time++;

        // Update waiting time for processes that are not completed and arrived before current time
        for (int i = 0; i < n; i++) {
            if (!is_completed[i] && processes[i].arrival_time < current_time && processes[i].remaining_time > 0) {
                processes[i].waiting_time++;
            }
        }
    }
}

int main() {
    Process processes[4] = {
        {1, 0, 6}, // Process ID, Arrival Time, Burst Time
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
Process P1 - Waiting Time: 5, Turnaround Time: 11, Completion Time: 11
Process P2 - Waiting Time: 0, Turnaround Time: 3, Completion Time: 5
Process P3 - Waiting Time: 1, Turnaround Time: 3, Completion Time: 7
Process P4 - Waiting Time: 5, Turnaround Time: 13, Completion Time: 19
*/