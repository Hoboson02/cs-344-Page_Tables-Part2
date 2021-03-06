// Name: Ryan Earl
// Project 10: Page Tables Part 2

#include "ptsim.h"
//
// Convert a page,offset into an address
//
int get_address(int page, int offset)
{
    return (page << PAGE_SHIFT) | offset;
}
//
// Initialize RAM
//
void initialize_mem(void)
{
    memset(mem, 0, MEM_SIZE);

    mem[0] = 1;
}
//
// Allocate a physical page
//
unsigned char get_page(void)
{
    for (int page_number = 0; page_number < PAGE_COUNT; page_number++) {
    	if (mem[page_number] == 0) {
            mem[page_number] = 1;
    		return page_number;
		}
	}

    return 0xff;
}

void set_page_table_entry(int page_table, int vpage, int page)
{
    int pt_addr = get_address(page_table, vpage);
    mem[pt_addr] = page;
}

//
// Allocate pages for a new process
//
void new_process(int proc_num, int page_count)
{
    int page_table = get_page();

    mem[64 + proc_num] = page_table;

    for (int vpage = 0; vpage < page_count; vpage++) {
        int page = get_page();

        set_page_table_entry(page_table, vpage, page);

        
	}
}

//
// ALGORITHM TO DEALLOCATE A PAGE
//
void deallocate_page(int page_num) {
    mem[page_num] = 0; //Set the value at address p in zeropage to 0
}
//
// ALGORITHM TO KILL A PROCESS
//
void kill_process(int page) {
    int page_table_page = get_page_table(page); // Get the page table page for this process

    for (int i = 0; i < PAGE_SIZE; i++) { //For each entry in the page table
        int page_address = get_address(page_table_page, i);
        if (mem[page_address] != 0) {//If it's not 0 
            int page_num = mem[page_address];
            deallocate_page(page_num);//Deallocate that page
            mem[page_address] = 0;
        }
    deallocate_page(page_table_page);
    }
}

//
// CONVERTING A VIRTUAL ADDRESS TO A PHYSICAL ADDRESS
//


int get_physical_address(int proc_num, int virtual_addr) {
    int virtual_page = virtual_addr >> 8; //Get the virtual page (see code above)
    int page_table_addr = get_address(get_page_table(proc_num), 0); //Converting page into an address
    int offset = virtual_addr & 255; // Get the offset
    int phys_page = mem[page_table_addr + virtual_page];// Get the physical page from the page table

    int phys_addr = (phys_page << 8) | offset;// Build the physical address from the phys page and offset

    // Return it
    return phys_addr;
}


//
// ALGORITHM TO STORE A VALUE AT A VIRTUAL ADDRESS
//
void store_value(int proc_num, int virt_addr, int value) {
    int phys_addr = get_physical_address(proc_num, virt_addr);
    mem[phys_addr] = value;
    printf("Store proc %d: %d => %d, value=%d\n",
    proc_num, virt_addr, phys_addr, value);
}



void print_page_free_map(void)
{
    printf("--- PAGE FREE MAP ---\n");
    for (int i = 0; i < 64; i++) {
        int addr = get_address(0, i);
        printf("%c", mem[addr] == 0? '.': '#');
        if ((i + 1) % 16 == 0)
            putchar('\n');
    }
}

//
// ALGORITHM TO LOAD A VALUE FROM A VIRTUAL ADDRESS
//
void load_value(int proc_num, int virt_addr) {
    int phys_addr = get_physical_address(proc_num, virt_addr);
    int value = mem[phys_addr];
    printf("Load proc %d: %d => %d, value=%d\n",
    proc_num, virt_addr, phys_addr, value);

    print_page_free_map();
}

//
// Print the address map from virtual pages to physical
//
void print_page_table(int proc_num)
{
    printf("--- PROCESS %d PAGE TABLE ---\n", proc_num);
    // Get the page table for this process
    int page_table = get_page_table(proc_num);
    // Loop through, printing out used pointers
    for (int i = 0; i < PAGE_COUNT; i++) {
        int addr = get_address(page_table, i);
        int page = mem[addr];
        if (page != 0) {
            printf("%02x -> %02x\n", i, page);
        }
    }
}
//
// Main -- process command line
//
int main(int argc, char *argv[])
{
    assert(PAGE_COUNT * PAGE_SIZE == MEM_SIZE);
    if (argc == 1) {
        fprintf(stderr, "usage: ptsim commands\n");
        return 1;
    }
    
    initialize_mem();
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "np") == 0) {
            int proc_num = atoi(argv[++i]);
            int pages = atoi(argv[++i]);
            new_process(proc_num, pages);
        }
        else if (strcmp(argv[i], "pfm") == 0) {
            print_page_free_map();
        }
        else if (strcmp(argv[i], "ppt") == 0) {
            int proc_num = atoi(argv[++i]);
            print_page_table(proc_num);
        }
        else if (strcmp(argv[i], "kp") == 0) { // kill process n and free all its pages.
            int proc_num = atoi(argv[++i]);
            kill_process(proc_num);
        }
        else if (strcmp(argv[i], "sb") == 0) { // For process n at virtual address a, store the value b.
            int proc_num = atoi(argv[++i]);
            int virt_addr = atoi(argv[++i]);
            int value = atoi(argv[++i]);
            store_value(proc_num, virt_addr, value);
        }
        else if(strcmp(argv[i], "lb") == 0) { // For process n, get the value at virtual address a.
            int proc_num = atoi(argv[++i]);
            int virt_addr = atoi(argv[++i]);
            load_value(proc_num, virt_addr);
        }
    }
}
