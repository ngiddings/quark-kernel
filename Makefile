.PHONY: all
all: quark.iso

quark.iso: src/quark-kernel
	cp src/quark-kernel rootfs/apps
	grub-mkrescue -o $@ rootfs

src/quark-kernel:
	make -C src

.PHONY: clean
clean:
	make -C src clean
	rm -f quark.iso
