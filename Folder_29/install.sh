#!/bin/bash
set -e

echo "Building module..."
make

DEST=/lib/modules/$(uname -r)/kernel/drivers/mydrivers
sudo mkdir -p $DEST

echo "Installing gpiobtn.ko..."
sudo cp -v gpiobtn.ko $DEST/

echo "Running depmod..."
sudo depmod -a

echo "Loading driver..."
sudo modprobe gpiobtn || sudo insmod gpiobtn.ko

echo "Done! Check:"
echo "  dmesg | tail"
echo "  ls -l /dev/gpiobtn"

