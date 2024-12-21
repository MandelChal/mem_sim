# mem_sim
 Memory Simulation System This project is a memory simulation system implemented in C. The system simulates a memory management unit (MMU) that handles memory accesses, swapping, and page table management.

Memory Simulation System
This project is a memory simulation system implemented in C. The system simulates a memory management unit (MMU) that handles memory accesses, swapping, and page table management.

Table of Contents
Memory Simulation System
Table of Contents
Overview
Files
Functions
Usage
License
Overview
The memory simulation system provides a simple way to simulate how a computer's memory is managed, including loading pages from a simulated executable file into memory, swapping pages to a swap file, and managing a page table. The system supports read and write operations to the simulated memory, printing the current state of the physical memory, swap file, and page table.

Files
mem_sim.h: Header file containing the definitions and function prototypes.
mem_sim.c: Implementation file containing the main logic of the memory simulation system.
Functions
Initialization
init_system: Initializes the simulation system, including opening the executable and swap files, initializing the page table, and setting the sizes of the text, data, and BSS/heap/stack segments.
Memory Access
load: Loads a byte from the specified address in the simulated memory.
store: Stores a byte to the specified address in the simulated memory.
Page Management
load_page: Loads a page from the executable file or swap file into memory.
find_available_page_in_swap: Finds an available page in the swap file for swapping.
Printing Functions
print_memory: Prints the current state of the physical memory.
print_swap: Prints the current state of the swap file.
print_page_table: Prints the current state of the page table.
Usage
Compile the code:

bash
Copy code
gcc -o mem_sim mem_sim.c
Run the simulation:

bash
Copy code
./mem_sim <exe_file_name> <swap_file_name> <text_size> <data_size> <bss_heap_stack_size>
exe_file_name: Path to the executable file to be simulated.
swap_file_name: Path to the swap file to be used.
text_size: Size of the text segment.
data_size: Size of the data segment.
bss_heap_stack_size: Size of the BSS/heap/stack segment.
Example:

bash
Copy code
./mem_sim program.exe swapfile.swap 1024 512 2048
