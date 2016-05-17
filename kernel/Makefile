CFLAGS?=-O2 -g
CPPFLAGS?=
LDFLAGS?=
LIBS?=

LD=/opt/cross64/bin/x86_64-elf-ld
CC=clang --target=x86_64-elf
CXX=clang++ --target=x86_64-elf
AS=$(CC) -D__ASSEMBLY__

KERNEL_DIR:=$(PROJECT_DIR)/kernel

CFLAGS:=$(CFLAGS) -ffreestanding -fno-builtin -nostdlib -nostdinc -Wall -Wextra -I $(KERNEL_DIR) -I $(PROJECT_DIR)/EXTERNAL_HEADERS -DKERNEL -D__LP64__
CPPFLAGS:=$(CPPFLAGS) -nostdinc++
LDFLAGS:=$(LDFLAGS)
LIBS:=$(LIBS) -nostdlib


CFLAGS:=$(CFLAGS) $(KERNEL_ARCH_CFLAGS)
CPPFLAGS:=$(CPPFLAGS) $(KERNEL_ARCH_CPPFLAGS)
LDFLAGS:=$(LDFLAGS) $(KERNEL_ARCH_LDFLAGS)	-Wl,-e,_start							\
											-Wl,-sectalign,.text,0x1000 			\
                                            -Wl,-sectalign,.data,0x1000 			\
											-Wl,-sectalign,.bss,0x1000 				\

LIBS:=$(LIBS) -L $(PROJECT_DIR)/libc -lk

KERNELL_DIR:=$(KERNEL_DIR)/kernel
MODULES_DIR:=$(KERNEL_DIR)/modules
X86_64_DIR:=$(KERNEL_DIR)/x86_64
I386_DIR:=$(KERNEL_DIR)/i386

OBJS:=\
$(X86_64_DIR)/start.o           \
$(X86_64_DIR)/boot_pt.o         \
$(I386_DIR)/gdt.o               \
$(KERNELL_DIR)/kernel.o         \
$(KERNELL_DIR)/tty.o            \
$(KERNELL_DIR)/kmalloc.o        \
$(KERNELL_DIR)/command.o        \
$(MODULES_DIR)/PS2Controller.o  \
$(KERNEL_DIR)/asmlib.o          \
$(KERNEL_DIR)/arch.o            \

CRTI_OBJ:=$(I386_DIR)/crti.o
CRTBEGIN_OBJ:=$(shell $(CC) $(CFLAGS) $(LDFLAGS) -print-file-name=crtbegin.o)
CRTEND_OBJ:=$(shell $(CC) $(CFLAGS) $(LDFLAGS) -print-file-name=crtend.o)
CRTN_OBJ:=$(I386_DIR)/crtn.o

ALL_OUR_OBJS:=\
$(CRTI_OBJ)   \
$(OBJS)       \
$(CRTN_OBJ)   \

OBJ_LINK_LIST:=\
$(CRTI_OBJ)     \
$(OBJS)         \
$(CRTN_OBJ)     \
#$(CRTBEGIN_OBJ) \
$(CRTEND_OBJ)   \

all: install

.PHONY: all clean clean-all install run

BetaOS.kernel: clean-all $(OBJ_LINK_LIST)
	$(LD) -o $@ $(OBJ_LINK_LIST) $(LIBS)

%.o: %.c
	$(CC) -c $< -o $@ -std=gnu11 $(CFLAGS) -I $(PROJECT_DIR)/libc/include

%.o: %.cpp
	$(CXX) -c $< -o $@ $(CFLAGS) $(CPPFLAGS) -I $(PROJECT_DIR)/libc/include

%.o: %.S
	$(AS) -c $< -o $@ -I $(PROJECT_DIR)/kernel -nostdinc -nostdinc++ -nostdlib -ffreestanding -fno-builtin

clean:
	rm -f BetaOS.kernel $(OBJS) $(ALL_OUR_OBJS) *.o */*.o */*/*.o *.kernel */*.kernel */*/*.kernel

clean-all: clean
	rm -rf $(PROJECT_DIR)/product/BetaOS.iso $(PROJECT_DIR)/product/boot/BetaOS.kernel

install: BetaOS.kernel
	cp BetaOS.kernel $(PROJECT_DIR)/product/boot/
	grub-mkrescue -o $(PROJECT_DIR)/product/BetaOS.iso $(PROJECT_DIR)/product
	make clean

run: install
	$(PROJECT_DIR)/run
