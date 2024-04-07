//OUTPUT OF THE CODE IS AT THE BOTTOM
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define N 5
// Define the time quanta for each queue
#define HIGH_PRIORITY_QUANTUM 8
#define MEDIUM_PRIORITY_QUANTUM 15
#define BOOST_INTERVAL 110

// Process structure
typedef struct {
    char PID[3];
    int arrival_time;
    int burst_time;
    int remaining_time;
    int completion_time;
    int turnaround_time;
    int priority;
    int first_executed_time;
    int last_executed_time;
} Process;

// Queue node
typedef struct QueueNode {
    Process* process;
    struct QueueNode* next;
} QueueNode;

// Queue structure
typedef struct {
    QueueNode* front;
    QueueNode* rear;
    int time_quantum;
} Queue;

// Define the queues for each priority level
Queue high_priority_queue = {NULL, NULL, HIGH_PRIORITY_QUANTUM};
Queue medium_priority_queue = {NULL, NULL, MEDIUM_PRIORITY_QUANTUM};
Queue low_priority_queue = {NULL, NULL, INT_MAX}; // FCFS, so no time quantum

// Function prototypes
void initialize_processes(Process processes[]);
void execute_MLFQ(Process processes[]);
void print_statistics(Process processes[]);
void print_queue_state(int current_time);
void print_single_queue_state(Queue* queue, const char* queue_name);
void enqueue(Queue* queue, Process* process);
Process* dequeue(Queue* queue);
void process_completed(Process* process, int time);
int all_processes_completed(Process processes[]);
void boost_priorities(Process processes[], Queue* high_priority, int time);

// Main function
int main() {
    Process processes[N] = {
        {"P1", 0, 6, 0, 0, 0, 1, 0},
        {"P2", 0, 14, 0, 0, 0, 1, 0},
        {"P3", 0, 30, 0, 0, 0, 1, 0},
        {"P4", 0, 65, 0, 0, 0, 1, 0},
        {"P5", 103, 2, 0, 0, 0, 1, 0}
    };

    initialize_processes(processes);
    execute_MLFQ(processes);
    print_statistics(processes);
    
    return 0;
}

void initialize_processes(Process processes[]) {
    for (int i = 0; i < N; i++) {
        processes[i].remaining_time = processes[i].burst_time;
        processes[i].first_executed_time = -1; // Initialize to -1
        processes[i].priority = (processes[i].arrival_time == 0) ? 1 : 2;
    }
}

void execute_MLFQ(Process processes[]) {
    int current_time = 0;
    int boost_time = BOOST_INTERVAL;

    // Initial state of the queues
    print_queue_state(current_time);

    while (!all_processes_completed(processes)) {
        // Check for new arrivals and enqueue them based on their priority
        for (int i = 0; i < N; i++) {
            if (processes[i].arrival_time == current_time) {
                if (processes[i].priority == 1) {
                    enqueue(&high_priority_queue, &processes[i]);
                } else if (processes[i].priority == 2) {
                    enqueue(&medium_priority_queue, &processes[i]);
                } else {
                    enqueue(&low_priority_queue, &processes[i]);
                }
            }
        }

        // Check for priority boosting
        if (current_time >= boost_time) {
            boost_priorities(processes, &high_priority_queue, current_time);
            boost_time += BOOST_INTERVAL;
        }

        Process* current_process = NULL;
        int time_quantum = INT_MAX; // Default for FCFS (low priority queue)

        // Execute processes from the highest priority queue available
        if (high_priority_queue.front != NULL) {
            current_process = dequeue(&high_priority_queue);
            time_quantum = HIGH_PRIORITY_QUANTUM;
        } else if (medium_priority_queue.front != NULL) {
            current_process = dequeue(&medium_priority_queue);
            time_quantum = MEDIUM_PRIORITY_QUANTUM;
        } else if (low_priority_queue.front != NULL) {
            current_process = dequeue(&low_priority_queue);
            // time_quantum remains INT_MAX for FCFS behavior
        }

        // Simulate process execution
        if (current_process != NULL) {
            if (current_process->first_executed_time == -1) {
                current_process->first_executed_time = current_time;
            }

            int execution_time = (current_process->remaining_time < time_quantum) ? current_process->remaining_time : time_quantum;
            current_time += execution_time;
            current_process->remaining_time -= execution_time;
            current_process->last_executed_time = current_time;

            // Check if the process is completed
            if (current_process->remaining_time == 0) {
                process_completed(current_process, current_time);
            } else {
                // Decrease priority and move to the next lower priority queue
                current_process->priority++;
                if (current_process->priority == 2) {
                    enqueue(&medium_priority_queue, current_process);
                } else if (current_process->priority > 2) {
                    enqueue(&low_priority_queue, current_process);
                }
            }
        } else {
            // If no processes are available to run, increment the current time
            current_time++;
        }
        print_queue_state(current_time);
    }
}

void print_statistics(Process processes[]) {
    printf("PID\tArrival Time\tBurst Time\tService Time\tCompletion Time\tTurnaround Time\n");
    for (int i = 0; i < N; i++) {
        // Service time is the first execution time minus the arrival time
        int service_time = processes[i].first_executed_time - processes[i].arrival_time;
        printf("%s\t%d\t\t%d\t\t%d\t\t%d\t\t%d\n",
               processes[i].PID,
               processes[i].arrival_time,
               processes[i].burst_time,
               service_time,
               processes[i].completion_time,
               processes[i].turnaround_time);
    }
}

void print_queue_state(int current_time) {
    printf("At t = %d, ", current_time);
    
    // Print the state of each queue
    print_single_queue_state(&high_priority_queue, "Q1");
    print_single_queue_state(&medium_priority_queue, "Q2");
    print_single_queue_state(&low_priority_queue, "Q3");
    printf("\n");
}

void print_single_queue_state(Queue* queue, const char* queue_name) {
    printf("%s contains ", queue_name);
    if (queue->front == NULL) {
        printf("empty");
    } else {
        QueueNode* node = queue->front;
        while (node != NULL) {
            printf("%s(%d)", node->process->PID, node->process->remaining_time);
            node = node->next;
            if (node != NULL) {
                printf(", ");
            }
        }
    }
    printf("; ");
}

void enqueue(Queue* queue, Process* process) {
    QueueNode* new_node = (QueueNode*)malloc(sizeof(QueueNode));
    new_node->process = process;
    new_node->next = NULL;

    if (queue->rear == NULL) {
        queue->front = queue->rear = new_node;
    } else {
        queue->rear->next = new_node;
        queue->rear = new_node;
    }
}

Process* dequeue(Queue* queue) {
    if (queue->front == NULL) {
        return NULL;
    }

    QueueNode* temp = queue->front;
    Process* process = temp->process;
    queue->front = queue->front->next;

    if (queue->front == NULL) {
        queue->rear = NULL;
    }

    free(temp);
    return process;
}

void process_completed(Process* process, int time) {
    process->completion_time = time;
    process->turnaround_time = time - process->arrival_time;
}

int all_processes_completed(Process processes[]) {
    for (int i = 0; i < N; i++) {
        if (processes[i].remaining_time > 0) {
            return 0; // False
        }
    }
    return 1; // True
}

void boost_priorities(Process processes[], Queue* high_priority, int time) {
    // Move all processes to the high priority queue and reset their priority
    for (int i = 0; i < N; i++) {
        if (processes[i].remaining_time > 0 && processes[i].priority != 1) {
            processes[i].priority = 1;
            enqueue(high_priority, &processes[i]);
        }
    }
}

/*
OUTPUT IS:
At t = 0, Q1 contains empty; Q2 contains empty; Q3 contains empty; 
At t = 6, Q1 contains P2(14), P3(30), P4(65); Q2 contains empty; Q3 contains empty; 
At t = 14, Q1 contains P3(30), P4(65); Q2 contains P2(6); Q3 contains empty; 
At t = 22, Q1 contains P4(65); Q2 contains P2(6), P3(22); Q3 contains empty; 
At t = 30, Q1 contains empty; Q2 contains P2(6), P3(22), P4(57); Q3 contains empty; 
At t = 36, Q1 contains empty; Q2 contains P3(22), P4(57); Q3 contains empty; 
At t = 51, Q1 contains empty; Q2 contains P4(57); Q3 contains P3(7); 
At t = 66, Q1 contains empty; Q2 contains empty; Q3 contains P3(7), P4(42); 
At t = 73, Q1 contains empty; Q2 contains empty; Q3 contains P4(42); 
At t = 115, Q1 contains empty; Q2 contains empty; Q3 contains empty; 
At t = 117, Q1 contains empty; Q2 contains empty; Q3 contains empty; 
PID     Arrival Time    Burst Time      Service Time    Completion Time Turnaround Time
P1      0               6               0               6               6
P2      0               14              6               36              36
P3      0               30              14              73              73
P4      0               65              22              115             115
P5      103             2               12              117             14
*/