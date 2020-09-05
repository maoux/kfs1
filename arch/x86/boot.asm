;declare constant for the multiboot header
MBALIGN		equ 1 << 0 ;aligns modules on 4Kb pages boundaries
MEMINFO		equ 1 << 1 ;if mem_* is present, include it
FLAGS		equ MBALIGN | MEMINFO 
MAGIC		equ 0x1BADB002
CHECKSUM	equ -(MAGIC + FLAGS)

;declare a multiboot header that marks the program as a kernel
section .multiboot
align 4
	dd MAGIC
	dd FLAGS
	dd CHECKSUM

;declare a stack into bss section
section .bss
align 16
stack_bottom:
	resb 16384
stack_top:


section .text
	global _start:function (_start.end - _start)

	extern keyboard_handler_main

_start:

	mov esp, stack_top

	push ebx
	push eax

	extern kmain
	call kmain

	;clear interupt flags, processor
	;won't handle maskable interupts anymore
	cli

.hang:
	hlt
	jmp .hang

.end:
