objs = math.o cstring.o error.o pagetableentry.o physicalmemoryallocator.o multiboot2header.o entry.o quarkkernel.o
link_script = linker.ld
quark_bin = qkernel

CXX = i686-elf-g++
CC = i686-elf-gcc

CPPFLAGS += -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti

all: $(objs)
	echo $(PATH)
	i686-elf-g++ -o $(quark_bin) -T $(link_script) -ffreestanding -nostdlib -O2 $(objs) -lgcc

clean:
	rm *.o
	rm -f $(quark_bin)
