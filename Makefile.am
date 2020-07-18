SUBDIRS = src

quark.iso: src/quark-kernel rootfs/boot/grub/grub.cfg
	mkdir -p rootfs/apps
	cp src/quark-kernel rootfs/apps
	grub-mkrescue rootfs/ -o quark.iso