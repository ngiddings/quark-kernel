#include "elf.h"
#include "pageallocator.h"
#include "mmgr.h"
#include "string.h"
#include "types/status.h"

const uint32_t elf_magic_number = 0x464c457f;

int load_program(struct elf_file_header_t *elf, struct page_stack_t *page_stack)
{
    struct elf_program_header_t *program_header = (struct elf_program_header_t*)((void*)elf + elf->phoffset);
    unsigned int count = elf->phcount;
    while(count > 0)
    {
        if(program_header->type == ELF_LOAD)
        {
            void *d = program_header->vaddr, *s = (void*)elf + program_header->offset;
            for(size_t n = 0; n < program_header->memsize; n += page_size)
            {
                physaddr_t page = reserve_page(page_stack);
                if(page == S_OUT_OF_MEMORY)
                {
                    return S_OUT_OF_MEMORY;
                }
                int status = map_page(page_stack, d, page, PAGE_RW | PAGE_USERMODE | PAGE_EXECUTABLE);
                switch(status)
                {
                case S_OUT_OF_MEMORY:
                    return status;
                case S_OUT_OF_BOUNDS:
                    return status;
                case S_OK:
                    memcpy(d, s, page_size);
                }
            }
        }
        count--;
        program_header = (struct elf_program_header_t*)((void*)program_header + elf->phsize);
    }
}