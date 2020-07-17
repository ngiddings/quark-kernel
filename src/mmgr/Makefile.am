noinst_LIBRARIES = libmmgr.a
libmmgr_a_SOURCES = memorymap.cpp buddyallocator.cpp addressspace.cpp
libmmgr_a_CPPFLAGS = -ffreestanding -O0 -Wall -fno-exceptions -fno-rtti -ggdb

if x86
libmmgr_a_SOURCES += x86/pagetableentry.cpp
endif