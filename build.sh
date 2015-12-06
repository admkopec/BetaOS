#!/bin/bash
export PREFIX="/opt/cross";
export TARGET=i686-elf;
export PATH="/opt/cross/bin:$PATH";

if ! i686-elf-as boot.s -o build/boot.o; then
exit 1;
fi
if ! i686-elf-g++ -c kernel.cpp -o build/kernel.o -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti; then
exit 2;
fi
if ! i686-elf-gcc -T build/linker.ld -o build/product/BetaOS.bin -ffreestanding -O2 -nostdlib build/boot.o build/kernel.o -lgcc; then
exit 3;
fi
cp build/product/BetaOS.bin build/product/ISO/boot;
cd build/product/ISO;
rm BetaOS.iso;
grub-mkrescue -o BetaOS.iso ./;
exit 0;