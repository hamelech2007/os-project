#!/bin/bash

# Start gdb in TUI mode, execute the remote target command, and load the symbol file
gdb -tui -ex "target extended-remote localhost:1234" -ex "symbol-file dist/x86_64/kernel.bin"