#include <stdio.h>

typedef struct {
    int process_id;
    int arrival_time;
    int burst_time;
    int waiting_time;
    int turnaround_time;
    int completion_time;
    int service_time;
} Process;

void calculateTimes(Process processes[], int n) {
    int current_time = 0;
    float total_waiting_time = 0.0;
    float total_turnaround_time = 0.0;

    for (int i = 0; i < n; i++) {
        // Service time is the time at which process starts executing
        if (current_time < processes[i].arrival_time)
            current_time = processes[i].arrival_time;
        
        processes[i].service_time = current_time;
        
        // Waiting time is service time minus arrival time
        processes[i].waiting_time = processes[i].service_time - processes[i].arrival_time;
        
        // Turnaround time is burst time plus waiting time
        processes[i].turnaround_time = processes[i].burst_time + processes[i].waiting_time;
        
        // Completion time is turnaround time plus arrival time
        processes[i].completion_time = processes[i].turnaround_time + processes[i].arrival_time;
        
        // Update current_time for the next process
        current_time += processes[i].burst_time;

        // Keep track of total waiting and turnaround time for average calculation
        total_waiting_time += processes[i].waiting_time;
        total_turnaround_time += processes[i].turnaround_time;
        
        // Print the times for each process
        printf("Process P%d - Waiting Time: %d, Turnaround Time: %d, Completion Time: %d\n", 
               processes[i].process_id, processes[i].waiting_time, processes[i].turnaround_time, processes[i].completion_time);
    }

    // Calculate and print average waiting time and average turnaround time
    printf("Average Waiting Time: %.2f\n", total_waiting_time / n);
    printf("Average Turnaround Time: %.2f\n", total_turnaround_time / n);
}

int main() {
    Process processes[4] = {
        {1, 0, 10}, // Process ID, Arrival Time, Burst Time
        {2, 2, 3},
        {3, 4, 7},
        {4, 6, 5}
    };

    calculateTimes(processes, 4);

    return 0;
}

/*
Process P1 - Waiting Time: 0, Turnaround Time: 10, Completion Time: 10
Process P2 - Waiting Time: 8, Turnaround Time: 11, Completion Time: 13
Process P3 - Waiting Time: 9, Turnaround Time: 16, Completion Time: 20
Process P4 - Waiting Time: 14, Turnaround Time: 19, Completion Time: 25
Average Waiting Time: 7.75
Average Turnaround Time: 14.00
*/
