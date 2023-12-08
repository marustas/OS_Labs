#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define DISK_SIZE 5000
#define REQUESTS 1000

//Initialize the functions representing the scheduling algorithms
int fcfs(int requests[], int initial_head);
int sstf(int requests[], int initial_head);
int scan(int requests[], int initial_head);
int cscan(int requests[], int initial_head);
int look(int requests[], int initial_head);
int clook(int requests[], int initial_head);
void generate_requests(int requests[]);

int main(int argc, char *argv[])
{
    //Create an array of 1000 random requests and initialize a head
    int requests[REQUESTS];
    generate_requests(requests);
    int initial_head;

    // Check for valid command line argument for initial head position
    if (argc != 2)
    {
        printf("Usage: %s <initial_head_position>\n", argv[0]);
        return -1;
    }
    // Set the initial head to the command line argument
    initial_head = atoi(argv[1]);
    if (initial_head < 0 || initial_head >= DISK_SIZE)
    {
        printf("Invalid initial head position. Must be between 0 and %d.\n", DISK_SIZE - 1);
        return -1;
    }

    // Seed random number generator
    srand(time(NULL));

    // Apply and print results of each algorithm
    printf("Total head movement for FCFS: %d\n", fcfs(requests, initial_head));
    printf("Total head movement for SSTF: %d\n", sstf(requests, initial_head));
    printf("Total head movement for SCAN: %d\n", scan(requests, initial_head));
    printf("Total head movement for C-SCAN: %d\n", cscan(requests, initial_head));
    printf("Total head movement for LOOK: %d\n", look(requests, initial_head));
    printf("Total head movement for C-LOOK: %d\n", clook(requests, initial_head));
    return 0;
}

// Function to compare two integers for qsort
int cmpfunc(const void *a, const void *b)
{
    return (*(int *)a - *(int *)b);
}

// FCFS Algorithm
int fcfs(int requests[], int head)
{
    //Go through each request sequentially in the specified sequence
    int total_movement = 0;
    for (int i = 0; i < REQUESTS; i++)
    {
        total_movement += abs(requests[i] - head);
        head = requests[i];
    }
    return total_movement;
}

// SSTF Algorithm
int sstf(int original_requests[], int head) {
    int requests[REQUESTS];
    //Copy the array
    for (int i = 0; i < REQUESTS; i++)
    {
        requests[i] = original_requests[i];
    }
    int head_movement = 0;
    int current_position = head;

    //Go through the closest cylinder
    for (int i = 0; i < REQUESTS; ++i)
    {
        int closest_index = -1;
        int closest_distance = DISK_SIZE + 1;

        for (int j = 0; j < REQUESTS; ++j)
        {
            if (requests[j] != -1)
            {
                int distance = abs(requests[j] - current_position);
                if (distance < closest_distance)
                {
                    closest_distance = distance;
                    closest_index = j;
                }
            }
        }
        //Add the head movement
        head_movement += closest_distance;
        current_position = requests[closest_index];

        requests[closest_index] = -1;
    }
    //Returnt the total number of head movements
    return head_movement;
}

// SCAN Algorithm
int scan(int requests[], int head)
{
    int head_movement = 0;
    int current_position = head;

    // Sort the requests array using qsort
    qsort(requests, REQUESTS, sizeof(int), cmpfunc);

    // Find the requests smaller than the head
    int current_index = 0;
    while (current_index < REQUESTS && requests[current_index] < current_position)
    {
        current_index++;
    }

    // Process requests in the direction of the smallest value first
    for (int i = current_index - 1; i >= 0; i--)
    {
        head_movement += abs(current_position - requests[i]);
        current_position = requests[i];
    }

    // Reverse direction and process requests in the direction of the largest value
    if (current_index < REQUESTS)
    {
        // Add the movement to the smallest end of the disk if there are requests in the other direction
        head_movement += current_position; // moving to 0
        current_position = 0;
    }

    for (int i = current_index; i < REQUESTS; i++)
    {
        head_movement += abs(current_position - requests[i]);
        current_position = requests[i];
    }
    //Return the total head movement
    return head_movement;
}

// CSCAN Algorithm
int cscan(int requests[], int head)
{
    int head_movement = 0;
    int current_position = head;

    // Sort in ascending order
    qsort(requests, REQUESTS, sizeof(int), cmpfunc);

    // Find the index where the current position is located
    int count = 0;
    for (int j = 0; j < REQUESTS; j++)
    {
        if (requests[j] < current_position)
        {
            count++;
        }
    }

    // Initialize two arrays for left and right requests
    int left[count];
    int right[REQUESTS - count];

    // Fill the left array with smaller requests
    for (int i = 0; i < count; ++i)
    {
        left[i] = requests[i];
    }

    // Fill the right array with greater requests
    for (int i = count; i < REQUESTS; ++i)
    {
        right[i - count] = requests[i];
    }

    // Process requests in the forward direction for the right array
    for (int i = 0; i < REQUESTS - count; ++i)
    {
        head_movement += abs(right[i] - current_position);
        current_position = right[i];
    }

    // Go to the start of the disk and set the current position as 0
    head_movement += abs(DISK_SIZE - current_position - 1);
    head_movement += DISK_SIZE - 1;

    current_position = 0;
    // Process requests in the left array
    for (int i = 0; i < count; i++)
    {
        head_movement += abs(left[i] - current_position);
        current_position = left[i];
    }
    //Return the total head movement
    return head_movement;
}

// LOOK Algorithm
int look(int requests[], int head)
{
    int head_movement = 0;
    int current_position = head;

    // Sort the requests array using qsort
    qsort(requests, REQUESTS, sizeof(int), cmpfunc);

    // Find the requests smaller than the head
    int current_index = 0;
    while (current_index < REQUESTS && requests[current_index] < current_position)
    {
        current_index++;
    }

    // Process requests in the direction of the largest value
    for (int i = current_index; i < REQUESTS; i++)
    {
        head_movement += abs(current_position - requests[i]);
        current_position = requests[i];
    }

    // Process requests in the direction of the smallest value first
    for (int i = current_index - 1; i >= 0; i--)
    {
        head_movement += abs(current_position - requests[i]);
        current_position = requests[i];
    }

    //Return the total head movement
    return head_movement;
}

// CLOOK Algorithm
int clook(int requests[], int head)
{
    int head_movement = 0;
    int current_position = head;

    // Sort the requests array using qsort
    qsort(requests, REQUESTS, sizeof(int), cmpfunc);

    // Find the requests smaller than the head
    int current_index = 0;
    while (current_index < REQUESTS && requests[current_index] < current_position)
    {
        current_index++;
    }

    // Process requests in the direction of the largest value
    for (int i = current_index; i < REQUESTS; i++)
    {
        head_movement += abs(current_position - requests[i]);
        current_position = requests[i];
    }
    
    //Go to the smallest value among requests and set it as current postion
    head_movement+= current_position - requests[0];
    current_position = requests[0];

    // Process requests in the direction of the smallest value first
    for (int i = 0; i < current_index; i++)
    {
        head_movement += abs(current_position - requests[i]);
        current_position = requests[i];
    }

    //Return the total head movement
    return head_movement;
}

// Function to generate random requests
void generate_requests(int requests[])
{
    for (int i = 0; i < REQUESTS; i++)
    {
        requests[i] = rand() % DISK_SIZE;
    }
}