#kernel_source_files := $(shell find src/impl/kernel -name *.c ! -name *32.c)
kernel_source_files := $(shell find src/impl/kernel -name *.c)
kernel_object_files := $(patsubst src/impl/kernel/%.c, build/kernel/%.o, $(kernel_source_files))

#kernel32_source_files := $(shell find src/impl/kernel -name *32.c)
#kernel32_asm_files :=  $(patsubst src/impl/kernel/%.c, build/kernel/%.asm, $(kernel32_source_files))
#kernel32_object_files := $(patsubst src/impl/kernel/%.c, build/kernel/%.o, $(kernel32_source_files))

x86_64_c_source_files := $(shell find src/impl/x86_64 -name *.c)
x86_64_c_object_files := $(patsubst src/impl/x86_64/%.c, build/x86_64/%.o, $(x86_64_c_source_files))

x86_64_asm_source_files := $(shell find src/impl/x86_64 -name *.asm)
x86_64_asm_object_files := $(patsubst src/impl/x86_64/%.asm, build/x86_64/%.o, $(x86_64_asm_source_files))

intf_c_headers_files_include_flag := $(shell find src/intf -type d -exec echo -I{} \; | tr '\n' ' ')


x86_64_object_files := $(x86_64_c_object_files) $(x86_64_asm_object_files)

$(kernel_object_files): build/kernel/%.o : src/impl/kernel/%.c
	mkdir -p $(dir $@) && \
	x86_64-elf-gcc -mcmodel=large -c -g -I src/intf $(intf_c_headers_files_include_flag) -ffreestanding $(patsubst build/kernel/%.o, src/impl/kernel/%.c, $@) -o $@

#$(kernel32_asm_files): build/kernel/%.asm : src/impl/kernel/%.c
#	mkdir -p $(dir $@) && \
	x86_64-elf-gcc -c -g -m32 -mpreferred-stack-boundary=3 -S -I src/intf $(intf_c_headers_files_include_flag) -ffreestanding $(patsubst build/kernel/%.asm, src/impl/kernel/%.c, $@) -o $@ && \
	sed -i '1i .code32' $@

#$(kernel32_object_files): build/kernel/%.o : build/kernel/%.asm
#	mkdir -p $(dir $@) && \
	x86_64-elf-as -g $(patsubst build/kernel/%.o, build/kernel/%.asm, $@) -o $@

$(x86_64_c_object_files): build/x86_64/%.o : src/impl/x86_64/%.c
	mkdir -p $(dir $@) && \
	x86_64-elf-gcc -mcmodel=large -c -g -I src/intf $(intf_c_headers_files_include_flag) -ffreestanding $(patsubst build/x86_64/%.o, src/impl/x86_64/%.c, $@) -o $@

$(x86_64_asm_object_files): build/x86_64/%.o : src/impl/x86_64/%.asm
	mkdir -p $(dir $@) && \
	x86_64-elf-as -g $(patsubst build/x86_64/%.o, src/impl/x86_64/%.asm, $@) -o $@

.PHONY: build-x86_64
build-x86_64: $(kernel_object_files) $(x86_64_object_files) #$(kernel32_object_files)
	mkdir -p dist/x86_64 && \
	x86_64-elf-ld -n -g -o dist/x86_64/kernel.bin -T targets/x86_64/linker.ld $(kernel_object_files) $(x86_64_object_files) && \
	cp dist/x86_64/kernel.bin targets/x86_64/iso/boot/kernel.bin && \
	grub-mkrescue /usr/lib/grub/i386-pc -o dist/x86_64/kernel.iso targets/x86_64/iso
#	x86_64-elf-ld -n -g -o dist/x86_64/kernel.bin -T targets/x86_64/linker.ld $(kernel_object_files) $(x86_64_object_files) $(kernel32_object_files) && \
	cp dist/x86_64/kernel.bin targets/x86_64/iso/boot/kernel.bin && \
	grub-mkrescue /usr/lib/grub/i386-pc -o dist/x86_64/kernel.iso targets/x86_64/iso