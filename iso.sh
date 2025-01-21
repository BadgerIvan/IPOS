#!/bin/sh
set -e
. ./build.sh

mkdir -p isodir
mkdir -p isodir/boot
mkdir -p isodir/boot/grub

cp sysroot/boot/ipos.kernel isodir/boot/ipos.kernel
cat > isodir/boot/grub/grub.cfg << EOF
menuentry "ipos" {
	multiboot /boot/ipos.kernel
}
EOF
grub-mkrescue -o ipos.iso isodir
