#include <linux/kernel.h>

#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/vmalloc.h>
#include <linux/highmem.h>
#include <linux/uaccess.h>
#include <linux/mm_types.h>

#include <asm/page.h>
#include <asm/pgtable.h>
#include <asm/current.h>


/* macros */
/* buffer[0] is used to store number of address intervals
 * format of each page: [start of page address][start of page frame address]
 * format of buffer: [total amounts][page 01][page 02][page 03] ...
 */
#define INFO_SIZE 2 // two addresses a set
#define NULL_FRAME 0lu // invalid physical address
#define MEMORY_SIZE 10000


/* functions */
unsigned long convert_to_physical(const struct mm_struct *const mm, const unsigned long virtual_addr);

void write_page_info(const struct mm_struct *const mm, unsigned long **const buffer, const unsigned long vir_start);

void write_memory_region(const struct mm_struct *const mm, unsigned long **const buffer, const unsigned long big_start, const unsigned long big_end);


/* main syscall */
asmlinkage void sys_linux_survey_TT(char *buffer) {
	unsigned long *ptr, *kbuffer;
	const struct vm_area_struct *cur_node; // current node in memory regions' list

	printk("start linux_survey_TT()!\n");
	
	kbuffer = vmalloc(MEMORY_SIZE * sizeof(unsigned long));
	ptr = kbuffer + 1;

	for (cur_node = current->mm->mmap; cur_node != NULL; cur_node = cur_node->vm_next) {
		if (cur_node->vm_start >= PAGE_OFFSET) {
			continue; // only need information from user address space
		}

		if (cur_node->vm_end - cur_node->vm_start > PAGE_SIZE) { // large than a page size
			write_memory_region(current->mm, &ptr, cur_node->vm_start, cur_node->vm_end);
		} else { // just a single page
			write_page_info(current->mm, &ptr, cur_node->vm_start);
		}
	}

	kbuffer[0] = (ptr - (kbuffer + 1)) / INFO_SIZE; // total page amount
	copy_to_user((unsigned long*) buffer, kbuffer, (ptr - kbuffer) * sizeof(unsigned long)); // write size

	vfree(kbuffer);
		
	printk("exit  linux_survey_TT()!\n");
}
/* exit syscall */


void write_page_info(const struct mm_struct *const mm, unsigned long **const buffer, const unsigned long vir_start) {

	// printk("start write_page_info(%p, %p, %08lx)\n", mm, buffer, vir_start);

	*((*buffer)++) = vir_start;
	*((*buffer)++) = convert_to_physical(mm, vir_start);

	// printk("exit  write_page_info()\n");
}


void write_memory_region(const struct mm_struct *const mm, unsigned long **const buffer, const unsigned long big_start, const unsigned long big_end) {
	unsigned long vir_start;

	// printk("start write_big_memory_region(%p, %p, %08lx, %08lx)\n", mm, buffer, big_start, big_end);

	for (vir_start = big_start; vir_start + PAGE_SIZE <= big_end; vir_start += PAGE_SIZE) {
		write_page_info(mm, buffer, vir_start);
	}

	// printk("exit  write_big_memory_region()\n");
}


unsigned long convert_to_physical(const struct mm_struct *const mm, const unsigned long vir) {
	unsigned long phys;
	pgd_t *pgd = NULL;
	pud_t *pud = NULL;
	pmd_t *pmd = NULL;
	pte_t *pte = NULL;
	struct page *page = NULL;

	// printk("start convert_to_physical(%p, %08lx)\n", mm, vir);

	pgd = pgd_offset(mm, vir);
	if (pgd_none(*pgd) || pgd_bad(*pgd)) {
		return NULL_FRAME;
	}

	pud = pud_offset(pgd, vir);
	if (pud_none(*pud) || pud_bad(*pud)) {
		return NULL_FRAME;
	}

	pmd = pmd_offset(pud, vir);
	if (pmd_none(*pmd) || pmd_bad(*pmd)) {
		return NULL_FRAME;
	}

	if (pmd_large(*pmd)) {
		printk("[!!] Fatal: extended paging found!!!\n");
	}

	pte = pte_offset_map(pmd, vir);
	if (pte == NULL) {
		return NULL_FRAME;
	}

	page = pte_page(*pte);
	if (page == NULL) {
		return NULL_FRAME;
	}

	phys = page_to_phys(page);
	pte_unmap(pte);

	// printk("exit  convert_to_physical() with %08lx\n", phys);

	return phys;
}
