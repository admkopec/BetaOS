CC=clang --target=x86_64-elf
CXX=clang++ --target=x86_64-elf
AR=/opt/cross64/bin/x86_64-elf-ar
AS=$(CC) -D__ASSEMBLY__

CFLAGS?=-O2 -g
CPPFLAGS?=
LDFLAGS?=
LIBS?=

DESTDIR?=
PREFIX?=/usr/local
EXEC_PREFIX?=$(PREFIX)
INCLUDEDIR?=$(PREFIX)/include
LIBDIR?=$(EXEC_PREFIX)/lib

CFLAGS:=$(CFLAGS) -Wall -Wextra -DKERNEL -DLIBC -D__LP64__ -I ~/Desktop/OS/kernel -I ~/Desktop/OS/EXTERNAL_HEADERS
CPPFLAGS:=$(CPPFLAGS) -I include
LIBK_CFLAGS:=$(CFLAGS) -ffreestanding -fno-builtin
LIBK_CPPFLAGS:=$(CPPFLAGS) -D__CPP__

CFLAGS:=$(CFLAGS) $(ARCH_CFLAGS)
CPPFLAGS:=$(CPPFLAGS) $(ARCH_CPPFLAGS)
LIBK_CFLAGS:=$(LIBK_CFLAGS) $(KERNEL_ARCH_CFLAGS)
LIBK_CPPFLAGS:=$(LIBK_CPPFLAGS) $(KERNEL_ARCH_CPPFLAGS)

FREEOBJS:=\
$(ARCH_FREEOBJS) 	\
stdio/printf.o 		\
stdio/putchar.o 	\
stdio/puts.o 			\
stdio/getchar.o 	\
stdio/gets.o 			\
stdlib/panic.o 		\
stdlib/error.o 		\
string/itoa.o 		\
string/strlen.o 	\
string/strcmp.o 	\
time/time.o 			\

HOSTEDOBJS:=\
$(ARCH_HOSTEDOBJS) \

OBJS:=\
$(FREEOBJS) 	\
$(HOSTEDOBJS) \

LIBK_OBJS:=$(FREEOBJS:.o=.libk.o)

BINARIES=libc.a libk.a

all: clean-all install

.PHONY: all clean-all clean install install-headers install-libs

libc.a: $(OBJS)
	$(AR) rcs $@ $(OBJS)

libk.a: $(LIBK_OBJS)
	$(AR) rcs $@ $(LIBK_OBJS)

%.o: %.cpp
	$(CXX) -c $< -o $@ $(CFLAGS) $(CPPFLAGS)

%.o: %.c
	$(CC) -c $< -std=gnu11 -o $@ $(CFLAGS) $(CPPFLAGS)

%.o: %.S
	$(AS) -c $< -o $@ -I $(PROJECT_DIR)/kernel -I $(PROJECT_DIR)/libc/include

%.libk.o: %.cpp
	$(CXX) -c $< -o $@ $(LIBK_CFLAGS) $(LIBK_CPPFLAGS)

%.libk.o: %.c
	$(CC) -c $< -o $@ -std=gnu11 $(LIBK_CFLAGS) $(LIBK_CPPFLAGS)

%.libk.o: %.S
	$(AS) -c $< -o $@ $(LIBK_CFLAGS) $(LIBK_CPPFLAGS)

clean:
	rm -rf $(OBJS) $(LIBK_OBJS) *.o */*.o */*/*.o

clean-all: clean
	rm -rf $(BINARIES)

install: $(BINARIES)
	make clean
