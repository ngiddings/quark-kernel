objs = x86/inthandlers.o x86/interruptdescriptor.o x86/idt.o x86/interrupts.o
archive = libinterrupts.a

$(archive): $(objs)
	i686-elf-ar rcs $@ $^

.PHONY: clean
clean:
	rm -f $(archive) $(objs)