/* SPDX-License-Identifier: GPL-2.0 */

OUTPUT_FORMAT("binary")

MEMORY
{
	ram(rwx) : ORIGIN = 0, LENGTH = 14M
	rom(rx)	 : ORIGIN = 0xe00000, LENGTH = 1M
}

SECTIONS
{
	.exception_vector : {
		. += 1K;
	} >ram

	.system_variable : {
		. += 1K;
	} >ram

	.stack : ALIGN(4) {
		_stack_bottom = .;
		. += 2K;
		_stack_top = .;
	} >ram

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
		*(.debug)
		*(.bss)
	}
}
