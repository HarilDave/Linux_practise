#!/bin/bash
set -e

echo "Unloading driver..."
sudo rmmod gpiobtn || true

echo "Removing module file..."
sudo rm -f /lib/modules/$(uname -r)/kernel/drivers/mydrivers/gpiobtn.ko

echo "Running depmod..."
sudo depmod -a

echo "Done. Driver uninstalled."

