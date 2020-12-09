#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([quark-kernel], [pre-alpha])
AM_INIT_AUTOMAKE([-Wall foreign subdir-objects])
AC_CONFIG_SRCDIR([src/allocator.cpp])
AC_CONFIG_HEADERS([src/config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AM_PROG_AS 
AM_PROG_AR
AC_PROG_RANLIB

# Checks for header files.
AC_CHECK_HEADERS([stddef.h stdint.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T

AM_CONDITIONAL([x86], [test $host = i686-elf])

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
