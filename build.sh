#!/bin/sh
export PREFIX="/opt/cross";
export TARGET=i686-elf;
export PATH="/opt/cross/bin:$PATH";
set -e
. ./headers.sh

for PROJECT in $PROJECTS; do
  DESTDIR="$PWD/sysroot" $MAKE -C $PROJECT install
done
