#!/bin/bash

# Check if any arguments are passed
if [ "$#" -eq 0 ]; then
    # No arguments passed, run the default command
    qemu-system-x86_64 -cdrom dist/x86_64/kernel.iso
elif [ "$1" == "debug" ]; then
    # Argument is "debug", run the debug command
    qemu-system-x86_64 -cdrom dist/x86_64/kernel.iso -s -S
else
    echo "Unknown argument: $1"
    exit 1
fi