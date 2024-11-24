#!/bin/bash

# Check if any arguments are passed
if [ "$#" -eq 0 ]; then
    # No arguments passed, run the default command
    qemu-system-x86_64 -cdrom dist/x86_64/kernel.iso -smp cores=2 -monitor stdio -drive file=disk.img,if=none,id=disk,format=raw -device ahci,id=ahci -device ide-hd,drive=disk,bus=ahci.0
elif [ "$1" == "debug" ]; then
    # Argument is "debug", run the debug command
    qemu-system-x86_64 -cdrom dist/x86_64/kernel.iso -smp cores=2 -monitor stdio -drive file=disk.img,if=none,id=disk,format=raw -device ahci,id=ahci -device ide-hd,drive=disk,bus=ahci.0 -no-reboot -no-shutdown -s -S
else
    echo "Unknown argument: $1"
    exit 1
fi