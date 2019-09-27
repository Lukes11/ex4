#!/bin/bash

dmesg -C
insmod proj2.ko int_str="1,2,3,4,5"
cat /proc/proj2
echo "Unloading..."
echo "Kernel Log Message:"
rmmod proj2
dmesg

