#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// Define the struct for a node in the linked list
struct node_t {
    int tickets;            // Number of tickets for this job
    struct node_t *next;    // Pointer to the next node
};

// Global variables
int gtickets = 0;           // Total number of tickets
struct node_t *head = NULL; // Head of the linked list

// Function prototypes
void insert(int tickets);
void print_list();
// Add other function prototypes as needed

int main(int argc, char *argv[]) {
    // Check if the correct number of command-line arguments is provided
    // If not, print usage and exit with error
    if (argc != 3) {
        fprintf(stderr, "usage: lottery <seed> <loops>\n");
        exit(1);
    }

    // Parse command-line arguments
    int seed = atoi(argv[1]);   // Seed for random number generation
    int loops = atoi(argv[2]);  // Number of simulation loops

    // Initialize the random number generator
    srandom(seed);

    // Populate the linked list with initial jobs, each with some number of tickets
    // Modify this part to allow user input for job details
    insert(70);
    insert(90);
    insert(35);

    // Print the initial list of jobs
    print_list();

    // Implement scheduling logic here
    
    // Loop for the specified number of simulation loops
    for (int i = 0; i < loops; i++) {
        int winning_ticket = random() % gtickets + 1; // Ensure we start from 1
        int sum = 0;
        struct node_t *current = head;
        while (current != NULL) {
            sum += current->tickets;
            if (sum >= winning_ticket) {
                break; // Found the winner
            }
            current = current->next;
        }
        if (current != NULL) { // Ensure we found a winner
            print_list();
            printf("winner: %d %d\n\n", winning_ticket, current->tickets);
        }
    }
        // Initialize variables for winner selection
        // Counter for ticket accumulation
        // Randomly select a winner 'winner'
        // Pointer to traverse the linked list '*current'

        // Loop through the linked list to find the winner

    return 0;
}

// Function to insert a new job into the linked list
void insert(int tickets) {
    struct node_t *new_node = (struct node_t*)malloc(sizeof(struct node_t));
    assert(new_node != NULL); // Ensure memory allocation didn't fail
    new_node->tickets = tickets;
    new_node->next = head;
    head = new_node;
    gtickets += tickets; // Update the global ticket count
}

// Function to print the contents of the linked list
void print_list() {
    struct node_t *current = head;
    printf("List: ");
    while (current != NULL) {
        printf("[%d] ", current->tickets);
        current = current->next;
    }
    printf("\n");
}
