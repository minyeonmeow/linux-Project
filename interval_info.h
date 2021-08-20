#include "page_info.h"

/* 
 **************************************************************************************************
 *************************************** GLOBAL DATA **********************************************
 **************************************************************************************************
*/
const char *const total_format    = "[#] total: %lu\n";

const char *const virtual_format  = "virtual  interval: [0x%08lx, 0x%08lx)\n";
const char *const physical_format = "physical interval: [0x%08lx, 0x%08lx)\n";

const char *const vir_phys_format = "virtual: [0x%08lx, 0x%08lx)-> physical: [0x%08lx, 0x%08lx)\n";
const char *const phys_vir_format = "physical: [0x%08lx, 0x%08lx) <-virtual: [0x%08lx, 0x%08lx)\n";

/* 
 **************************************************************************************************
 ****************************************** DECLARATIONs ******************************************
 **************************************************************************************************
*/
// lsit the physical address intervals, list by virtual address order
void list_physical_intervals(const char *const filename);

// lsit the virtual address intervals
void list_virtual_intervals(const char *const filename);

/* 
 **************************************************************************************************
 ************************************* IMPLEMENTATIONS ********************************************
 **************************************************************************************************
*/
// lsit the physical address intervals, list by virtual address order
void list_physical_intervals(const char *const filename) {
    struct page_info pgif;
    unsigned long i, count, vir, phys, interval_length;
    
    FILE *const fin = open_file(filename, "r");
    const unsigned long SIZE = read_file_size(fin);

    i = 0;
    count = 0;

    if (SIZE != 0) {
        // read first frame
        do {
            read_page_info(fin, &pgif);
            ++i;
        } while (i < SIZE && pgif.phys_start == NULL_FRAME);

        vir = pgif.vir_start;
        phys = pgif.phys_start;
        interval_length = PAGE_SIZE;
    }

    while (i < SIZE) {
        // read a new frame
        do {
            read_page_info(fin, &pgif);
            ++i;
        } while (i < SIZE && pgif.phys_start == NULL_FRAME);

        if (i == SIZE && pgif.phys_start == NULL_FRAME) break;

        if (vir + interval_length == pgif.vir_start && phys + interval_length == pgif.phys_start) {
            interval_length += PAGE_SIZE;
        } else {
            printf(vir_phys_format, vir, vir + interval_length, phys, phys + interval_length);
            ++count;

            vir = pgif.vir_start;
            phys = pgif.phys_start;
            interval_length = PAGE_SIZE;
        }
    }
    check_EOF(fin);
    fclose(fin);

    printf(vir_phys_format, vir, vir + interval_length, phys, phys + interval_length);
    ++count;

    printf(total_format, count);
}

// lsit the virtual address intervals
void list_virtual_intervals(const char *const filename) {
    struct page_info pgif;
    unsigned long i, vir_start, vir_end, count;

    FILE *const fin = open_file(filename, "r");
    const unsigned long SIZE = read_file_size(fin);

    count = 0;

    if (SIZE != 0) { 
        // read first entry
        read_page_info(fin, &pgif);
        vir_start = pgif.vir_start;
        vir_end = vir_start + PAGE_SIZE;
    }

    for (i = 1; i < SIZE; ++i) {
        read_page_info(fin, &pgif);
        
        if (vir_end == pgif.vir_start) { 
            // continuous interval
            vir_end += PAGE_SIZE;
        } else { 
            // not continuous interval
            printf(virtual_format, vir_start, vir_end);
            ++count;
            // start new page
            vir_start = pgif.vir_start;
            vir_end = vir_start + PAGE_SIZE;
        }
    }
    check_EOF(fin);
    fclose(fin);

    // last virtual interval
    printf(virtual_format, vir_start, vir_end);
    ++count;

    printf(total_format, count);
}