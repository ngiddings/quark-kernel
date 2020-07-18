noinst_LIBRARIES = libmmgr.a
libmmgr_a_SOURCES = memorymap.cpp buddyallocator.cpp
libmmgr_a_CPPFLAGS = -ffreestanding -O0 -Wall -fno-exceptions -fno-rtti -ggdb

if x86
libmmgr_a_SOURCES += x86/pagetableentry.cpp x86/mmap.cpp
endif