#include "kernel.h"
#include "mmgr.h"
#include "stdio.h"
#include "elf.h"
#include "context.h"
#include "types/status.h"

size_t do_syscall(struct kernel_t *kernel, enum syscall_id_t id, size_t arg1, size_t arg2, size_t arg3)
{

}

int load_module(struct kernel_t *kernel, struct module_t *module)
{
    physaddr_t module_address_space = create_address_space(kernel->page_stack);
    load_address_space(module_address_space);
    void *const load_base = (void*)0x80000000;
    size_t load_offset = 0;
    for(physaddr_t p = module->start & ~(page_size - 1); p < module->end; p += page_size)
    {
        int status = map_page(kernel->page_stack, load_base + load_offset, p, PAGE_RW);
        switch(status)
        {
        case S_OUT_OF_MEMORY:
            panic("ran out of memory while mapping module");
        case S_OUT_OF_BOUNDS:
            panic("got out-of-bounds error while mapping module");
        }
        load_offset += page_size;
    }
    int status = load_program(load_base, kernel->page_stack);
    switch(status)
    {
    case S_OUT_OF_MEMORY:
        panic("ran out of memory while reading ELF file");
    case S_OUT_OF_BOUNDS:
        panic("got out-of-bounds error while reading ELF file");
    }
    void *module_entry = ((struct elf_file_header_t*)load_base)->entry;
    void *module_context = initialize_context(module_entry, kernel->page_stack);
    printf("loaded module with entry point %08x\n", (unsigned int)module_entry);
    load_offset = 0;
    for(physaddr_t p = module->start & ~(page_size - 1); p < module->end; p += page_size)
    {
        int status = unmap_page(load_base + load_offset);
        switch(status)
        {
        case S_OUT_OF_MEMORY:
            panic("ran out of memory while unmapping module");
        case S_OUT_OF_BOUNDS:
            panic("got out-of-bounds error while unmapping module");
        }
        load_offset += page_size;
    }
    int index = get_free_resource_slot(kernel->resource_table, kernel->page_stack);
    if(index < 0)
    {
        panic("no space left in resource table for module");
    }
    kernel->resource_table->array[index].type = RESOURCE_PROCESS;
    kernel->resource_table->array[index].process.priority = 1;
    kernel->resource_table->array[index].process.state = module_context;
    kernel->resource_table->array[index].process.page_table = current_address_space();
    queue_insert(kernel->priority_queue, &kernel->resource_table->array[index].process);
    return S_OK;
}

struct process_state_t *next_process(struct kernel_t *kernel, struct process_state_t *prev_state)
{  
    if(prev_state != NULL)
    {
        kernel->active_process->state = prev_state;
        queue_insert(kernel->priority_queue, kernel->active_process);
    }
    kernel->active_process = extract_min(kernel->priority_queue);
    if(kernel->active_process != NULL)
    {
        load_address_space(kernel->active_process->page_table);
        printf("entering process %08x cr3=%08x state=%08x.\n", kernel->active_process, kernel->active_process->page_table, kernel->active_process->state);
        return kernel->active_process->state;
    }
    panic("no processes available to enter!");
}

void panic(const char *message)
{
    printf("panic: %s", message);
    asm("cli");
    while(1) asm("hlt");
}
