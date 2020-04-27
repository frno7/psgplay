/* SPDX-License-Identifier: GPL-2.0 */

OUTPUT_FORMAT("binary")

ENTRY(_rom)

MEMORY
{
	sys(rwx) : ORIGIN = 0, LENGTH = 0x4000	/* 16 KiB */
	rom(rx)	 : ORIGIN = 0xe00000, LENGTH = 1M
}

SECTIONS
{
	.exception_vector : {
		. += 1K;
	} >sys

	.system_variable : {
		. += 1K;
	} >sys

	.stack : ALIGN(4) {
		_stack_bottom = .;
		. += 2K;
		_stack_top = .;
	} >sys

	.bss : ALIGN(4) {
		*(.bss)
		*(.bss.*)
	} >sys

	.text : ALIGN(4) {
		*(.text)
		*(.text.*)
	} >rom

	.rodata : ALIGN(4) {
		*(.rodata)
		*(.rodata.*)
	} >rom = 0

	.data : ALIGN(4) {
		*(.data)
		*(.data.*)
	} >rom = 0

	/DISCARD/ : {
		*(.comment)
		*(.debug*)
		*(.note.*)
	}
}
