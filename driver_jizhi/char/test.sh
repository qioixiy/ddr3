#!/bin/sh

make 
rmmod demo_chr_dev
insmod demo_chr_dev.ko
rm /dev/chr_dev
mknod /dev/chr_dev c 250 0

gcc test.c -o test
./test