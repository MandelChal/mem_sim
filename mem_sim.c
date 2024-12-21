
#include "mem_sim.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>

int five=0;
int free_frame=0;


int find_available_page_in_swap(sim_database* mem_sim) {
    char buffer[PAGE_SIZE];
    char zero_buf[PAGE_SIZE];
    memset(zero_buf, '0', PAGE_SIZE);

    // Go to the start of the swap file
    lseek(mem_sim->swapfile_fd, 0, SEEK_SET);
//    int i;

    for (int i = 0; i < SWAP_SIZE ; i++) {
        // Read a page from the swap file
        if (read(mem_sim->swapfile_fd, buffer, PAGE_SIZE) != PAGE_SIZE) {
            perror("Failed to read from swap file");
            return -1;
        }
        int j;
        for( j=0;j<8;j++){
            if(buffer[j]!='0')
                break;
        }
        if(j==8)
            return i/PAGE_SIZE;
        else
            i+=7;
    }

    // No available page found
    return -1;
}




sim_database* init_system(char exe_file_name[], char swap_file_name[], int text_size, int data_size, int bss_heap_stack_size) {
    sim_database* mem_sim = (sim_database*)malloc(sizeof(sim_database));
    if (!mem_sim) {
        perror("Failed to allocate memory for sim_database");
        exit(EXIT_FAILURE);
    }

    mem_sim->program_fd = open(exe_file_name, O_RDONLY);
    if (mem_sim->program_fd == -1) {
        perror("Failed to open executable file");
        free(mem_sim);
        exit(EXIT_FAILURE);
    }

    mem_sim->swapfile_fd = open(swap_file_name, O_RDWR | O_CREAT, 0600);
    if (mem_sim->swapfile_fd == -1) {
        perror("Failed to open/create swap file");
        close(mem_sim->program_fd);
        free(mem_sim);
        exit(EXIT_FAILURE);
    }

    char zero_buf[SWAP_SIZE];
    memset(zero_buf, '0', SWAP_SIZE);
    if (write(mem_sim->swapfile_fd, zero_buf, SWAP_SIZE) != SWAP_SIZE) {
        perror("Failed to initialize swap file");
        close(mem_sim->program_fd);
        close(mem_sim->swapfile_fd);
        free(mem_sim);
        exit(EXIT_FAILURE);
    }

    memset(mem_sim->main_memory, 0, MEMORY_SIZE);
    for (int i = 0; i < NUM_OF_PAGES; i++) {
        mem_sim->page_table[i].V = 0;
        mem_sim->page_table[i].D = 0;
        mem_sim->page_table[i].P = (i < (text_size / PAGE_SIZE)) ? 1 : 0;
        mem_sim->page_table[i].frame_swap = -1;
    }

    mem_sim->text_size = text_size;
    mem_sim->data_size = data_size;
    mem_sim->bss_heap_stack_size = bss_heap_stack_size;

    return mem_sim;
}

void load_page(sim_database* mem_sim, int page_num, int frame) {
    char buff[PAGE_SIZE];
    if (page_num < (mem_sim->text_size /PAGE_SIZE)) { // text_size / PAGE_SIZE
        // Text segment
        lseek(mem_sim->program_fd, page_num *PAGE_SIZE, SEEK_SET); // page_num * PAGE_SIZE
        read(mem_sim->program_fd, buff, PAGE_SIZE); // frame * PAGE_SIZE

    } else if (page_num < ((mem_sim->text_size + mem_sim->data_size) /PAGE_SIZE)) { // (text_size + data_size) / PAGE_SIZE
        // Data segment
        if(mem_sim->page_table[page_num].D==0) {
            lseek(mem_sim->program_fd, page_num *PAGE_SIZE, SEEK_SET); // page_num * PAGE_SIZE
            read(mem_sim->program_fd, buff, PAGE_SIZE); // frame * PAGE_SIZE}
        }
        else {
            lseek(mem_sim->swapfile_fd,  mem_sim->page_table[page_num].frame_swap*8, SEEK_SET); // page_num * PAGE_SIZE
            read(mem_sim->swapfile_fd, buff, PAGE_SIZE); // frame * PAGE_SIZE
            lseek(mem_sim->swapfile_fd,  mem_sim->page_table[page_num].frame_swap, SEEK_SET);
            char zeros[8];
            memset(zeros, '0', sizeof(zeros));
            // Write the 8 zeros to the file

            write(mem_sim->swapfile_fd, zeros, sizeof(zeros));
        }
    } else {

                memset(buff, '0', PAGE_SIZE);


    }
// mi lefanot
    if (five == 0) {
        memcpy(&mem_sim->main_memory[frame * PAGE_SIZE], buff, PAGE_SIZE);//write to memorry
        mem_sim->page_table[page_num].V = 1;
        mem_sim->page_table[page_num].frame_swap = frame;
        frame++;
        free_frame=frame;



        if (frame == 5 || free_frame ==5) {
            five = 1;
            frame =0;
            free_frame = 0;
        }
    } else {
        int pageToremove;
// who is in the frame
        for (int i = 0; i < NUM_OF_PAGES; i++) {
            if (mem_sim->page_table[i].frame_swap == frame && mem_sim->page_table[i].V == 1) {
                pageToremove=i;
                break;
            }
        }
        if (frame == 5 || free_frame ==5) {
            five = 1;
            frame =0;
            free_frame = 0;
        }
//        printf("%d pageToremove \n",pageToremove);
        if (mem_sim->page_table[pageToremove].D==0) {

            memcpy(&mem_sim->main_memory[frame*8], buff, PAGE_SIZE);//write to memorry
            mem_sim->page_table[pageToremove].V = 0;
            mem_sim->page_table[page_num].V = 1; // Mark page as valid
            mem_sim->page_table[page_num].frame_swap = frame;


        } else {

            char buffToSwap[PAGE_SIZE];
            memcpy(buffToSwap, &mem_sim->main_memory[free_frame*8], PAGE_SIZE); // read from memory


            int available_page = find_available_page_in_swap(mem_sim);
                    if (available_page == -1) {
                fprintf(stderr, "Error: No available page in swap file\n");
                exit(EXIT_FAILURE);
            }


            lseek(mem_sim->swapfile_fd, available_page * PAGE_SIZE, SEEK_SET);


            if (write(mem_sim->swapfile_fd, buffToSwap, PAGE_SIZE) != PAGE_SIZE) {
                perror("Failed to write to swap file");
                exit(EXIT_FAILURE);
            }
            mem_sim->page_table[pageToremove].frame_swap = available_page;
            memcpy(&mem_sim->main_memory[frame*8], buff, PAGE_SIZE); //write to memory
            mem_sim->page_table[pageToremove].V = 0;
            mem_sim->page_table[page_num].V = 1; // Mark page as valid
            mem_sim->page_table[page_num].frame_swap = frame;


        }
        frame++;
        free_frame=frame;

        if(frame==5 || free_frame==5) {
            free_frame=0;
            frame=0;
        }


    }
}

char load(sim_database* mem_sim, int address) {
    if (address < 0 || address >= (NUM_OF_PAGES * PAGE_SIZE)) {
        fprintf(stderr, "Error: Invalid address\n");
        return '\0';
    }

    int page_num = address >> 3; // address / PAGE_SIZE
    int offset = address & (PAGE_SIZE - 1); // address % PAGE_SIZE

    if (!mem_sim->page_table[page_num].V) {
//        printf("%d page_num\n",page_num);
        load_page(mem_sim, page_num, free_frame);
    }
//    free_frame++;
    return mem_sim->main_memory[mem_sim->page_table[page_num].frame_swap * PAGE_SIZE + offset];
}

void store(sim_database* mem_sim, int address, char value) {
    if (address < 0 || address >= (NUM_OF_PAGES * PAGE_SIZE)) {
        fprintf(stderr, "Error: Invalid address\n");
        return;
    }
    int page_num = address >> 3; // address / PAGE_SIZE
    int offset = address & (PAGE_SIZE - 1); // address % PAGE_SIZE
    if (mem_sim->page_table[page_num].P == 1) {
        fprintf(stderr, "Error: Page permissions disallow write access\n");
        return;
    }

    if (!mem_sim->page_table[page_num].V) {
        load_page(mem_sim, page_num, free_frame);

    }

    mem_sim->main_memory[(mem_sim->page_table[page_num].frame_swap << 3) + offset] = value;

    mem_sim->page_table[page_num].D = 1;
}

void print_memory(sim_database* mem_sim) {
    printf("\n Physical memory\n");
    for (int i = 0; i < MEMORY_SIZE; i++) {
        if (mem_sim->main_memory[i]==0)
            printf("[0]\n");
        else printf("[%c]\n", mem_sim->main_memory[i]);
    }
}

void print_swap(sim_database* mem_sim) {

    struct stat file_stat;

    // מקבלים את המידע על הקובץ
    if (fstat(mem_sim->swapfile_fd, &file_stat) == -1) {
        perror("fstat failed");
        return;
    }

    // בודקים אם הקובץ ריק
    if (file_stat.st_size == 0) {
        printf("\n Swap memory\n");
//        printf("The swap file is empty\n");
        printf("[0]\t");
        return;

    }
    else{
        char str[PAGE_SIZE];
        int i;
        printf("\n Swap memory\n");
        lseek(mem_sim->swapfile_fd, 0, SEEK_SET); // go to the start of the file
        while (read(mem_sim->swapfile_fd, str, PAGE_SIZE) == PAGE_SIZE) {
            for (i = 0; i < PAGE_SIZE; i++) {
                printf("[%c]\t", str[i]);
            }
            printf("\n");
        }}
}

void print_page_table(sim_database* mem_sim) {
    printf("\n Page table \n");
    printf("Valid  Dirty  Permission  Frame_swap\n");
    for (int i = 0; i < NUM_OF_PAGES; i++) {
        printf(" [%d]\t [%d]\t [%d]\t [%d]\n", mem_sim->page_table[i].V, mem_sim->page_table[i].D, mem_sim->page_table[i].P, mem_sim->page_table[i].frame_swap);
    }
}

void clear_system(sim_database* mem_sim) {
    close(mem_sim->program_fd);
    close(mem_sim->swapfile_fd);
    free(mem_sim);
}