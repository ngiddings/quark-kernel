objs = 	src/addressspace.o src/tty.o src/buddyallocator.o src/math.o \
	src/cstring.o src/pagetableentry.o src/multiboot2header.o \
	src/systeminfo.o src/memorymap.o src/entry.o src/quarkkernel.o
link_script = src/linker.ld
quark_bin = qkernel
quark_img = quark.iso

CXX = i686-elf-g++
CC = i686-elf-gcc

CPPFLAGS = -ffreestanding -O2 -Wall -fno-exceptions -fno-rtti -ggdb

.PHONY: all
all: $(quark_img)

$(quark_img): bin/$(quark_bin) rootfs/boot/grub/grub.cfg
	cp bin/$(quark_bin) rootfs/apps
	grub-mkrescue -o $@ rootfs

bin/$(quark_bin): $(objs) $(link_script)
	mkdir -p bin/
	$(CXX) -o $@ -T $(link_script) -ffreestanding -nostdlib -O0 $(objs) -lgcc

.PHONY: clean
clean:
	rm -f $(objs) bin/$(quark_bin) $(quark_img)
