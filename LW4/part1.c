#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define DISK_SIZE 200
#define REQUESTS 8


// Function to compare two integers for qsort
int cmpfunc(const void *a, const void *b)
{
    return (*(int *)a - *(int *)b);
}

int cscan(int requests[], int head)
{
    int head_movement = 0;
    int current_position = head;
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

    return head_movement;
}

int main(int argc, char *argv[])
{
    int requests[REQUESTS] = { 176, 79, 34, 60, 92, 11, 41, 114 };
    int initial_head;

    // Check for valid command line argument for initial head position
    if (argc != 2)
    {
        printf("Usage: %s <initial_head_position>\n", argv[0]);
        return -1;
    }

    initial_head = atoi(argv[1]);
    if (initial_head < 0 || initial_head >= DISK_SIZE)
    {
        printf("Invalid initial head position. Must be between 0 and %d.\n", DISK_SIZE - 1);
        return -1;
    }

    qsort(requests,REQUESTS, sizeof(int), cmpfunc);

    // Seed random number generator
    srand(time(NULL));

    // Apply and print results of each algorithm
    printf("Total head movement for C-SCAN: %d\n", cscan(requests, initial_head));
    // Call and print results for other algorithms

    return 0;
}
