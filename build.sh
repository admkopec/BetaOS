#!/bin/bash
export PREFIX="/opt/cross";
export TARGET=i686-elf;
export PATH="/opt/cross/bin:$PATH";

i686-elf-as boot.s -o build/boot.o;
i686-elf-g++ -c kernel.cpp -o build/kernel.o -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti;
i686-elf-gcc -T build/linker.ld -o build/product/BetaOS.bin -ffreestanding -O2 -nostdlib build/boot.o build/kernel.o -lgcc;
cp build/product/BetaOS.bin build/product/ISO/boot;
cd build/product/ISO;
rm BetaOS.iso;
grub-mkrescue -o BetaOS.iso ./;