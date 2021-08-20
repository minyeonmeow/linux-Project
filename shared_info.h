#include "page_info.h"

/* 
 **************************************************************************************************
 *************************************** GLOBAL DATA **********************************************
 **************************************************************************************************
*/
// formats to read/write files or print outcome
const char *const shared_frame_format = 
    "shared page frame: [0x%08lx, 0x%08lx)\n"
    "   process 1 page: [0x%08lx, 0x%08lx)\n"
    "   process 2 page: [0x%08lx, 0x%08lx)\n"
    "\n"
;

const char *const shared_interval_format = 
    "# shared physical interval: [0x%08lx, 0x%08lx)\n"
    "process 1 virtual interval: [0x%08lx, 0x%08lx)\n"
    "process 2 virtual interval: [0x%08lx, 0x%08lx)\n"
    "\n"
;

const char *const total_shared_frame_format = "[#] shared %lu page frames\n";
const char *const total_shared_format = "[#] shared %lu page frames, %lu intervals\n";

/* 
 **************************************************************************************************
 ****************************************** DECLARATIONs ******************************************
 **************************************************************************************************
*/
// structure to describe shared page frame
struct shared_frame_info;

// structure to describe shared intervals
struct shared_interval_info;

// read tow files to a array of shared page frames, sorted by physical address
void get_shared_frames(const char *const filename_1, const char *const filename_2, struct shared_frame_info **const buffer_dest, unsigned long *const size_dest);

// get shared intervals, sorted by physical address
void get_shared_intervals(const char *const filename_1, const char *const filename_2, struct shared_interval_info **const buffer_dest, unsigned long *const size_dest);

// lsit shared page frames by two process
void list_shared_frames(const char *const filename_1, const char *const filename_2);

// list shared intervls by two process
void list_shared_intervals(const char *const filename_1, const char *const filename_2);

/* 
 **************************************************************************************************
 ************************************* IMPLEMENTATIONS ********************************************
 **************************************************************************************************
*/
// custom structure to describe shared page
struct shared_frame_info {
    unsigned long phys;
    unsigned long vir_1;
    unsigned long vir_2;
};

// structure to describe shared intervals
struct shared_interval_info {
    unsigned long phys;
    unsigned long size;
    unsigned long vir_1;
    unsigned long vir_2;
};

// read tow files to a array of shared page frames, sorted by physical address
void get_shared_frames(const char *const filename_1, const char *const filename_2, struct shared_frame_info **const buffer_dest, unsigned long *const size_dest) {
    unsigned long i, j, size_1, size_2, count, size;
    struct page_info *buffer_1, *buffer_2;
    struct shared_frame_info *buffer;

    get_sorted_page_frames(filename_1, &buffer_1, &size_1);
    get_sorted_page_frames(filename_2, &buffer_2, &size_2);

    i = 0;
    j = 0;
    count = 0;
    size = 32;
    buffer = (struct shared_frame_info*) allocate(size, sizeof(struct shared_frame_info));

    while (i < size_1 && j < size_2) {
        if (count == size) {
            enlarge_buffer((void**) &buffer, &size, sizeof(struct shared_frame_info));
        }

        if (buffer_1[i].phys_start < buffer_2[j].phys_start) { // advance i
            ++i;
        } else if (buffer_1[i].phys_start > buffer_2[j].phys_start) { // advance j
            ++j;
        } else { // shared page
            buffer[count].vir_1 = buffer_1[i].vir_start;
            buffer[count].vir_2 = buffer_2[j].vir_start;
            buffer[count].phys = buffer_1[i].phys_start;
            ++count;
            ++i;
            ++j;
        }
    }

    free(buffer_1);
    free(buffer_2);

    *buffer_dest = buffer;
    *size_dest = count;
}

// lsit shared page frames by two process
void list_shared_frames(const char *const filename_1, const char *const filename_2) {
    unsigned long i, size;
    struct shared_frame_info *buffer;

    get_shared_frames(filename_1, filename_2, &buffer, &size);

    for (i = 0; i < size; ++i) {
        printf(shared_frame_format, 
            buffer[i].phys, buffer[i].phys + PAGE_SIZE, 
            buffer[i].vir_1, buffer[i].vir_1 + PAGE_SIZE, 
            buffer[i].vir_2, buffer[i].vir_2 + PAGE_SIZE
        );
    }

    free(buffer);

    printf(total_shared_frame_format, size);
}

// get shared intervals, sorted by physical address
void get_shared_intervals(const char *const filename_1, const char *const filename_2, struct shared_interval_info **const buffer_dest, unsigned long *const size_dest) {
    unsigned long i, size, count, frame_size, phys, vir_1, vir_2 , diff;
    struct shared_frame_info *frame_buffer;
    struct shared_interval_info *buffer;

    get_shared_frames(filename_1, filename_2, &frame_buffer, &frame_size);

    if (frame_size != 0) {
        // first shared frame
        phys = frame_buffer[0].phys;
        vir_1 = frame_buffer[0].vir_1;
        vir_2 = frame_buffer[0].vir_2;
        diff = 1;
    }

    count = 0;
    size = 32;
    buffer = (struct shared_interval_info*) allocate(size, sizeof(struct shared_interval_info));

    for (i = 1; i<frame_size; ++i) {
        if (phys + diff * PAGE_SIZE == frame_buffer[i].phys && vir_1 + diff * PAGE_SIZE == frame_buffer[i].vir_1 && vir_2 + diff * PAGE_SIZE == frame_buffer[i].vir_2) {
            // continuous
            ++diff;
        } else {
            // not continuous
            if (count == size) {
                enlarge_buffer((void**) &buffer, &size, sizeof(struct shared_interval_info));
            }

            buffer[count].phys = phys;
            buffer[count].vir_1 = vir_1;
            buffer[count].vir_2 = vir_2;
            buffer[count].size = diff * PAGE_SIZE;
            ++count;

            phys = frame_buffer[i].phys;
            vir_1 = frame_buffer[i].vir_1;
            vir_2 = frame_buffer[i].vir_2;
            diff = 1;
        }
    }

    free(frame_buffer);

    buffer[count].phys = phys;
    buffer[count].vir_1 = vir_1;
    buffer[count].vir_2 = vir_2;
    buffer[count].size = diff * PAGE_SIZE;
    ++count;

    *buffer_dest = buffer;
    *size_dest = count;
}

// list shared intervls by two process
void list_shared_intervals(const char *const filename_1, const char *const filename_2) {
    unsigned long i, size, amount;
    struct shared_interval_info *buffer;

    get_shared_intervals(filename_1, filename_2, &buffer, &size);

    amount = 0;
    for (i = 0; i < size; ++i) {
        printf(shared_interval_format, 
            buffer[i].phys, buffer[i].phys + buffer[i].size, 
            buffer[i].vir_1, buffer[i].vir_1 + buffer[i].size, 
            buffer[i].vir_2, buffer[i].vir_2 + buffer[i].size
        );
        amount += buffer[i].size;
    }

    printf(total_shared_format, amount / PAGE_SIZE, size);

    free(buffer);
}