#!/bin/bash
export PREFIX="/opt/cross";
export TARGET=i686-elf;
export PATH="/opt/cross/bin:$PATH";
##-g -O2 -w -trigraphs -fno-builtin  -fno-exceptions -fno-stack-protector -O0 -m32  -fno-rtti -nostdlib -nodefaultlibs
## Add linking and compiling every lib
if ! i686-elf-as boot.s -o build/boot.o -I ./include; then
exit 1;
fi
##if ! i686-elf-g++ -c kernel.cpp -o build/kernel.o -ffreestanding -fbuiltin -Wall -O2 -Wextra -fno-exceptions -m32 -fno-rtti -nostdlib -nodefaultlibs -I ./include; then
##exit 2;
##fi
if ! i686-elf-g++ -I ./include -g -O2 -w -trigraphs -ffreestanding -fno-builtin  -fno-exceptions -fno-stack-protector -O0 -m32  -fno-rtti -nostdlib -nodefaultlibs -c kernel.cpp -o build/kernel.o; then
exit 2;
fi
if ! i686-elf-g++ -I ./include -g -O2 -w -trigraphs -ffreestanding -fno-builtin  -fno-exceptions -fno-stack-protector -O0 -m32  -fno-rtti -nostdlib -nodefaultlibs -c ./include/stdio.cpp -o build/stdio.o; then
exit 2;
fi
##if ! i686-elf-gcc -T build/linker.ld -o build/product/BetaOS.bin -ffreestanding -fbuiltin -Wall -O2 -Wextra -nostdlib -m32 -static -L ./build -nodefaultlibs build/boot.o build/kernel.o -lgcc; then
##exit 3;
##fi
if ! i686-elf-ld build/boot.o build/kernel.o -melf_i386 -static -L ./ -T ./build/linker.ld -o ./build/product/BetaOS.bin; then
exit 3;
fi
cp build/product/BetaOS.bin build/product/ISO/boot;
cd build/product/ISO;
rm BetaOS.iso;
grub-mkrescue -o BetaOS.iso ./;
exit 0;
