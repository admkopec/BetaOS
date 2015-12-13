#!/bin/sh
set -e
. ./build.sh

mkdir -p product
mkdir -p product/boot
mkdir -p product/boot/grub

cp sysroot/boot/BetaOS.kernel product/boot/BetaOS.kernel
cat > product/boot/grub/grub.cfg << EOF
set timeout=1
set default=0
menuentry "BetaOS" {
	multiboot /boot/BetaOS.kernel
    boot
}
EOF
grub-mkrescue -o product/BetaOS.iso product

./clean.sh
