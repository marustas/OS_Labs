#include <stdio.h>
#include <stdlib.h>

#define CYLINDERS 5000
#define REQUESTS 10

int sstf(int requests[], int initial_position);
int scan(int requests[], int initial_position);
int c_scan(int requests[], int initial_position);
int look(int requests[], int initial_position);
int c_look(int requests[], int initial_position);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <initial_position>\n", argv[0]);
        return 1;
    }

    //Setup the initial position of the disk head
    int initial_position = atoi(argv[1]);
    //Inititalize an array of 1000 requestes
    int requests[REQUESTS];

    // Generate 1000 random requests
    for (int i = 0; i < REQUESTS; ++i) {
        requests[i] = rand() % CYLINDERS;
        printf('%d \t', requests[i]);
    }


    printf("Initial position: %d\n", initial_position);
    //Run the disk-scheduling algorithms
    fcfs(requests, initial_position);
    sstf(requests, initial_position);
    scan(requests, initial_position);
    c_scan(requests, initial_position);
    look(requests, initial_position);
    c_look(requests, initial_position);

    printf("Total number of head movements made by:\n");
    printf("FCFS: %d", fcfs(requests, initial_position));
    printf("SSTF: %d", fcfs(requests, initial_position));
    printf("SCAN: %d", fcfs(requests, initial_position));
    printf("C-SCAN: %d", fcfs(requests, initial_position));
    printf("LOOK: %d", fcfs(requests, initial_position));
    printf("C-LOOK: %d", fcfs(requests, initial_position));
    return 0;
}

int fcfs(int requests[], int initial_position) {
    //Initial number of head movements
    int head_movement = 0;
    int current_position = initial_position;

    for (int i = 0; i < REQUESTS; ++i) {
        head_movement += abs(requests[i] - current_position);
        current_position = requests[i];
    }

    return head_movement;
}

void quicksort(int array[REQUESTS],int low,int high) {

int x, y, p, temp;

if(low<high) {

    p=low;
    x=low;
    y=high;

    while(x<y){
        while(array[x]<=array[p]&&x<high)
        x++;
            while(array[y]>array[p])
            y--;
            if(x<y){
                temp=array[x];
                array[x]=array[y];
                array[y]=temp;
            }
        }
        temp=array[p];
        array[p]=array[y];
        array[y]=temp;

        quicksort(array,low,y-1);
        quicksort(array,y+1,high);
    }
}

int sstf(int requests[], int initial_position) {
    //Initial number of head movements
    int head_movement = 0;
    int current_position = initial_position;
    for(int i =0; i < REQUESTS; i++){

    }
}

int scan(int requests[], int initial_position) {
    //Initial number of head movements
    int head_movement = 0;
    int current_position = initial_position;
}

int c_scan(int requests[], int initial_position) {
    //Initial number of head movements
    int head_movement = 0;
    int current_position = initial_position;
}

int look(int requests[], int initial_position) {
    //Initial number of head movements
    int head_movement = 0;
    int current_position = initial_position;
}

int c_look(int requests[], int initial_position) {
    //Initial number of head movements
    int head_movement = 0;
    int current_position = initial_position;
}
