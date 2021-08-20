#include <unistd.h>
#include "generic.h"

/* 
 **************************************************************************************************
 ********************************************* MACROS *********************************************
 **************************************************************************************************
*/
#define NULL_FRAME 0lu // invalid physical address
#define SYSCALL_NUMBER 351 // syscall number of linux-3.10.104
#define PAGE_SIZE getpagesize() // page size

/* 
 **************************************************************************************************
 *************************************** GLOBAL DATA **********************************************
 **************************************************************************************************
*/
// formats to read/write files or print outcome
const char *const header_format  = "[ virtual  ]->[ physical ], total: %lu";
const char *const content_format = "[0x%08lx]->[0x%08lx]";

/* 
 **************************************************************************************************
 ****************************************** DECLARATIONs ******************************************
 **************************************************************************************************
*/
// requested syscall: void linux_survey_TT(char*)
void linux_survey_TT(char *buffer);

// custom data structure, describe a page and its corresponding physical address
struct page_info;

// compare virtual address for sorting
int compare_virtual(const void *const ptr_1, const void *const ptr_2);

// compare physical address for sorting
int compare_physical(const void *const ptr_1, const void *const ptr_2);

// read how many entries in the file
unsigned long read_file_size(FILE *const fin);

// write how many entries in the file
void write_file_size(FILE *const fout, const unsigned long SIZE);

// read memory chunk from file
void read_page_info(FILE *const fout, struct page_info *const pgif);

// write memory chunk to file
void write_page_info(FILE *const fout, const struct page_info *const pgif);

// write syscall result to file
void write_buffer(const void *const array, const char *const filename);

// read file to a array of sorted page frames, ignore zero values
void get_sorted_page_frames(const char *const filename, struct page_info **const buffer_dest, unsigned long *const size_dest);

// show the percentages of the virtual addresses that have physical memory assigned to them
void show_assigned_percentage(const char *const filename);

/* 
 **************************************************************************************************
 ************************************* IMPLEMENTATIONS ********************************************
 **************************************************************************************************
*/
// requested syscall: void linux_survey_TT(char*)
void linux_survey_TT(char *buffer) {
    syscall(SYSCALL_NUMBER, buffer);
}

// custom data structure, describe continuous virtual interval and corresponding continuous physical interval
struct page_info {
    unsigned long vir_start;
    unsigned long phys_start;
};

// compare virtual address for sorting
int compare_page_virtual(const void *const ptr_1, const void *const ptr_2) {
    const struct page_info *const a = (struct page_info*) ptr_1;
    const struct page_info *const b = (struct page_info*) ptr_2;

    if (a->vir_start < b->vir_start) {
        return -1;
    } else if (a->vir_start > b->vir_start) {
        return 1;
    } else {
        return 0;
    }
}

// compare physical address for sorting
int compare_page_physical(const void *const ptr_1, const void *const ptr_2) {
    const struct page_info *const a = (struct page_info*) ptr_1;
    const struct page_info *const b = (struct page_info*) ptr_2;

    if (a->phys_start < b->phys_start) {
        return -1;
    } else if (a->phys_start > b->phys_start) {
        return 1;
    } else {
        return 0;
    }
}

// read how many entries in the file
unsigned long read_file_size(FILE *const fin) {
    unsigned long size;

    if (fscanf(fin, header_format, &size) != 1) {
        fatal("reading file in read_file_size(), step 1");
    }
    if (fgetc(fin) != '\n') {
        fatal("reading file in read_file_size(), step 2");
    }

    return size;
}

// write how many entries in the file
void write_file_size(FILE *const fout, const unsigned long SIZE) {
    fprintf(fout, header_format, SIZE);
    fprintf(fout, "\n");
}

// read memory chunk from file
void read_page_info(FILE *const fin, struct page_info *const pgif) {
    if (fscanf(fin, content_format, &(pgif->vir_start), &(pgif->phys_start)) != 2) {
        fatal("reading file in read_page_info(), step 1");
    }
    if (fgetc(fin) != '\n') {
        fatal("reading file in read_page_info(), step 2");
    }
}

// write memory chunk to file
void write_page_info(FILE *const fout, const struct page_info *const pgif) {
    fprintf(fout, content_format, pgif->vir_start, pgif->phys_start);
    fprintf(fout, "\n");
}

// write syscall result to file
void write_buffer(const void *const array, const char *const filename) {
    unsigned long i;
    struct page_info pgif;
    const unsigned long *ptr, *const buffer = (const unsigned long*) array;

    FILE *const fout = open_file(filename, "w");
    write_file_size(fout, buffer[0]);

    ptr = buffer + 1;
    for (i = 0; i < buffer[0]; ++i) {
        pgif.vir_start = *(ptr++);
        pgif.phys_start = *(ptr++);
        write_page_info(fout, &pgif);
    }

    fclose(fout);
}

// read file to a array of sorted page frames, ignore zero values
void get_sorted_page_frames(const char *const filename, struct page_info **const buffer_dest, unsigned long *const size_dest) {
    unsigned long i, count, size;
    struct page_info *buffer, pgif;

    FILE *const fin = open_file(filename, "r");
    const unsigned long SIZE = read_file_size(fin);

    count = 0;
    size = 32;
    buffer = (struct page_info*) allocate(size, sizeof(struct page_info));

    for (i=0; i<SIZE; ++i) {
        read_page_info(fin, &pgif);

        if (pgif.phys_start != NULL_FRAME) { // present in memory
            if (count == size) {
                enlarge_buffer((void**) &buffer, &size, sizeof(struct page_info));
            }

            buffer[count].vir_start = pgif.vir_start;
            buffer[count].phys_start = pgif.phys_start;
            ++count;
        }
    }
    check_EOF(fin);
    fclose(fin);

    qsort(buffer, count, sizeof(struct page_info), compare_page_physical);

    *size_dest = count;
    *buffer_dest = buffer;
}

// show the percentages of the virtual addresses that have physical memory assigned to them
void show_assigned_percentage(const char *const filename) {
    struct page_info pgif;
    unsigned long i, vir_amount, phys_amount;

    FILE *const fin = open_file(filename, "r");
    const unsigned long SIZE = read_file_size(fin);

    vir_amount = SIZE * PAGE_SIZE;
    phys_amount = 0;
    for (i=0; i<SIZE; ++i) {
        read_page_info(fin, &pgif);

        if (pgif.phys_start != 0) {
            phys_amount += PAGE_SIZE;
        }
    }
    check_EOF(fin);
    fclose(fin);

    printf("total virtual  memory: %9lu bytes\n", vir_amount);
    printf("total physical memory: %9lu bytes\n", phys_amount);
    printf("assigned percentage: %.2lf%%\n", (((double) phys_amount) / vir_amount) * 100.0);
}