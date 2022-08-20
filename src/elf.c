#include "elf.h"
#include "mmgr.h"
#include "string.h"
#include "types/status.h"

const uint32_t elf_magic_number = 0x464c457f;

int load_program(struct elf_file_header_t *elf)
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
                physaddr_t page = reserve_page();
                if(page == ENOMEM)
                {
                    return ENOMEM;
                }
                int status = map_page(d + n, page, PAGE_RW | PAGE_USERMODE | PAGE_EXECUTABLE);
                switch(status)
                {
                case ENOMEM:
                    return status;
                case EOUTOFBOUNDS:
                    return status;
                case ENONE:
                    memcpy(d + n, s + n, n + page_size < program_header->memsize ? page_size : program_header->memsize - n);
                }
            }
        }
        count--;
        program_header = (struct elf_program_header_t*)((void*)program_header + elf->phsize);
    }
    return ENONE;
}