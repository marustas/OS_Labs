#include <stdio.h>
#include <stdlib.h>

#define NUM_OF_ADDRESSES 1000 // number of the given logical addresses in the addresses.txt
#define OFFSET_MASK 0xFF      // to extract rightmost 8 bits
#define PAGE_SHIFT 8          // shift const to get the page number
#define NUM_PAGES 256         // number of pages the virtual memory is divided into
#define FRAME_SIZE 256        // size of each frame in bytes
#define NUM_FRAMES 256        // number of frames the physical memory is divided into
#define TLB_SIZE 16           // translation lookaside buffer size
#define PHYS_MEM_SIZE (NUM_FRAMES * FRAME_SIZE)

// below are global variables and functions,
// separated by parts in order of which the lab assignment was done

// struct to contain the info about each logical address
typedef struct
{
    int logical_address;
    int page_number;
    int offset;
} AddressInfo;

// simulate physical memory: byte-sized array
// values are signed as seen in correct.txt
char physical_memory[PHYS_MEM_SIZE];

// PART 4
// create a page table
int page_table[NUM_PAGES];

// page fault handler
// for frame allocation strategy we will use first-available frame allocation
// page fault handler does not account for running out of frames
// (i.e. next_free_frame going beyond 255)
// due to the simplicity of our case (only 1000 addresses)
int handle_page_fault(int page_number, FILE *backing_store)
{
    static int next_free_frame = 0;             // static var to keep stack of the next frame
    int read_offset = page_number * FRAME_SIZE; // read offset from the backing store
    fseek(backing_store, read_offset, SEEK_SET);
    // load the frame from backing store specified by read_offset to the next free frame in physical memory
    fread(&physical_memory[next_free_frame * FRAME_SIZE], sizeof(char), FRAME_SIZE, backing_store);

    // update the page table
    page_table[page_number] = next_free_frame;
    // move onto the next free frame
    next_free_frame++;
    // return the frame that was written
    return next_free_frame - 1;
}

// TLB implementation
// TLB entry structure
typedef struct
{
    int page_number;
    int frame_number;
    int valid; // when just created, all the tlb entries will be invalid
} TLBEntry;

TLBEntry tlb[TLB_SIZE]; // define the TLB

// initialization
void initialize_tlb()
{
    for (int i = 0; i < TLB_SIZE; i++)
    {
        tlb[i].valid = 0; // tlb entries are initially invalid
    }
}

// update the tlb (tlb miss handler)
// replacing strategy: fifo
void update_tlb(int page_number, int frame_number)
{
    static int next_tlb_index = 0;
    tlb[next_tlb_index].page_number = page_number;
    tlb[next_tlb_index].frame_number = frame_number;
    tlb[next_tlb_index].valid = 1;
    next_tlb_index = (next_tlb_index + 1) % TLB_SIZE;
}

// check the tlb
int check_tlb(int page_number)
{
    for (int i = 0; i < TLB_SIZE; i++)
    {
        if (tlb[i].valid && tlb[i].page_number == page_number)
        {
            return tlb[i].frame_number; // tlb hit
        }
    }
    return -1; // tlb miss
}

int main()
{
    // open the backing store file
    FILE *backing_store;
    backing_store = fopen("./data/BACKING_STORE.bin", "rb");
    if (backing_store == NULL)
    {
        perror("Error opening BACKING_STORE.bin");
        return -1;
    }
    // initialize the tlb
    initialize_tlb();

    // read all the logical addresses from addresses.txt
    FILE *file;
    int logical_address_main;
    int logical_addresses_arr[NUM_OF_ADDRESSES];

    file = fopen("./data/addresses.txt", "r");
    if (file == NULL)
    {
        perror("Error opening file");
        return -1;
    }

    int i = 0;
    while (fscanf(file, "%d", &logical_address_main) != EOF)
    {
        logical_addresses_arr[i] = logical_address_main;
        i++;
    }
    printf("Successfully read the logical addresses into array\n");

    fclose(file);

    // parse the logical addresses, store as array of structs containing
    // all the neccessary info about the logical addresses
    AddressInfo addresses[NUM_OF_ADDRESSES];
    for (i = 0; i < NUM_OF_ADDRESSES; i++)
    {
        addresses[i].logical_address = logical_addresses_arr[i];
        addresses[i].offset = logical_addresses_arr[i] & OFFSET_MASK;
        addresses[i].page_number = (logical_addresses_arr[i] >> 8) & OFFSET_MASK;
    }

    // simulating physical memory
    // we will simulate it as byte-sized array (defined above the main)
    // below is the initialization of the memory with an arbitrary value
    for (i = 0; i < PHYS_MEM_SIZE; ++i)
    {
        physical_memory[i] = 0;
    }

    // creating a page table
    // definition is in the global scope
    // initialize all page table elements to -1
    for (i = 0; i < NUM_PAGES; i++)
    {
        page_table[i] = -1; // -1 indicates that the page is not in memory
    }


    int tlb_frame;
    int frame_number;
    int physical_address;
    char value;
    int page_fault_count = 0;
    int tlb_hit_count = 0;
    for (i = 0; i < NUM_OF_ADDRESSES; ++i)
    {
        tlb_frame = check_tlb(addresses[i].page_number);
        if (tlb_frame != -1) // tlb hit
        {
            frame_number = tlb_frame;

            // hit counts
            tlb_hit_count++;
        }
        else // tlb miss
        {
            // look up the frame number in page table
            frame_number = page_table[addresses[i].page_number];
        }
        if (frame_number == -1) // page fault
        {
            // page fault logging disabled to match the output format in correct.txt
            // uncomment if needed

            // printf("Page fault at address %d (page number %d)\n",
            //        addresses[i].logical_address, addresses[i].page_number);

            // access the backing store
            frame_number = handle_page_fault(addresses[i].page_number, backing_store);
            update_tlb(addresses[i].page_number, frame_number);

            // page fault counts
            page_fault_count++;
        }
        else // frame number found in page table
        {
            update_tlb(addresses[i].page_number, frame_number);
        }

        physical_address = (frame_number * FRAME_SIZE) + addresses[i].offset;
        value = physical_memory[physical_address];
        printf("Logical Address: %d, Physical Address: %d, Value: %d\n",
               addresses[i].logical_address, physical_address, value);
    }

    // calculate and print the statistics
    printf("Page fault rate: %d%%\n", page_fault_count * 100 / NUM_OF_ADDRESSES);
    printf("TLB hit rate: %d%%\n", tlb_hit_count * 100 / NUM_OF_ADDRESSES);

    // last steps
    fclose(backing_store); // close the backing store file that was held open
    return 0;
}
