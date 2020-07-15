objs = addressspace.o tty.o buddyallocator.o math.o cstring.o pagetableentry.o multiboot2header.o systeminfo.o memorymap.o pio.o entry.o quarkkernel.o

CXX = i686-elf-g++
CC = i686-elf-gcc

CXXFLAGS = -ffreestanding -mgeneral-regs-only -O0 -Wall -fno-exceptions -fno-rtti -ggdb
LDFLAGS = -T linker.ld -lgcc -nostdlib

quark-kernel: $(objs) linker.ld interrupts/libinterrupts.a
	$(CXX) -o $@ $(LDFLAGS) interrupts/libinterrupts.a $(objs)

interrupts/libinterrupts.a:
	make -C interrupts CXX="$(CXX)" CC="$(CC)" CXXFLAGS="$(CXXFLAGS)"

.PHONY: clean
clean:
	make -C interrupts clean
	rm -f $(objs) quark-kernel