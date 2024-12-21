#include "mem_sim.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "mem_sim.h"

int main() {
    sim_database* mem_sim = init_system("exec_file", "swap_file", 40, 40, 120);
    if (mem_sim == NULL) {

        exit(1);
    }

    // Load and store operations
    char val;

    val = load (mem_sim , 0);
    val = load (mem_sim , 8);
    val = load (mem_sim , 16);
    val = load (mem_sim , 24);
    val = load (mem_sim , 32);
    val = load (mem_sim , 40);
    store(mem_sim ,42,'B');
    val = load (mem_sim , 48);
    store(mem_sim ,49,'A');
    val = load (mem_sim , 56);
    val = load (mem_sim , 64);
    val = load (mem_sim , 0);
    val = load (mem_sim , 8);
    val = load (mem_sim , 16);
    val = load (mem_sim , 24);
    val = load (mem_sim , 32);
    store(mem_sim ,45,'A');
    val = load (mem_sim , 56);
    val = load (mem_sim , 64);
    val = load (mem_sim , 0);
    val = load (mem_sim , 8);
    val = load (mem_sim , 16);
    store(mem_sim ,150,'A');
    val = load (mem_sim , 56);
    val = load (mem_sim , 64);
    val = load (mem_sim , 0);
    val = load (mem_sim , 8);
    val = load (mem_sim , 16);
    store(mem_sim ,64,'A');
    val = load (mem_sim , 24);
    val = load (mem_sim , 160);
    val = load (mem_sim , 172);
    val = load (mem_sim , 191);
    val = load (mem_sim , 199);
    val = load (mem_sim , 150);
    store(mem_sim ,199,'b');
    store(mem_sim ,198,'b');
    val = load (mem_sim , 0);
    val = load (mem_sim , 8);
    val = load (mem_sim , 16);
    val = load (mem_sim , 24);
    val = load (mem_sim , 32);


    print_memory(mem_sim);
    print_page_table(mem_sim);
    print_swap(mem_sim);
    clear_system(mem_sim);
    printf("%d ",val);

    return 0;
}