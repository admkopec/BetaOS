#!/bin/bash
export PREFIX="/opt/cross";
export TARGET=i686-elf;
export PATH="/opt/cross/bin:$PATH";
##-g -O2 -w -trigraphs -fno-builtin  -fno-exceptions -fno-stack-protector -O0 -m32  -fno-rtti -nostdlib -nodefaultlibs
if ! i686-elf-as boot.s -o build/boot.o -Iinclude; then
exit 1;
fi
if ! i686-elf-g++ -c kernel.cpp -o build/kernel.o -ffreestanding -fbuiltin -Wall -O2 -Wextra -fno-exceptions -m32 -fno-rtti -nostdlib -nodefaultlibs -I sysroot/include; then
exit 2;
fi
if ! i686-elf-gcc -T build/linker.ld -o build/product/BetaOS.bin -ffreestanding -fbuiltin -Wall -O2 -Wextra -nostdlib -m32 -nodefaultlibs build/boot.o build/kernel.o -lgcc; then
exit 3;
fi
cp build/product/BetaOS.bin build/product/ISO/boot;
cd build/product/ISO;
rm BetaOS.iso;
grub-mkrescue -o BetaOS.iso ./;
exit 0;
