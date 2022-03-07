// Name: Ryan Earl
// Project 9: Page Tables Part 1

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
    mem[MEM_SIZE] = 0;
    mem[0] = 1;
}
//
// Allocate a physical page
//
unsigned char get_page(void)
{
    for (int page_number = 0; page_number <= PAGE_COUNT; page_number ++) {
    	if (mem[page_number] == 0) {
            mem[page_number] = 1;
    		return page_number;
		}
	}

    return 0xff;
}
//
// Allocate pages for a new process
//
void new_process(int proc_num, int page_count)
{
    int page_table = get_page();

    mem[64 + proc_num] = page_table;

    for (int i = 0; i < page_count; i++) {
        int new_page = get_page();
        int pt_addr = get_address(page_table, i);
        mem[pt_addr] = new_page;
	}
}
//
// Print the free page map
//
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
    }
}
