/* SPDX-License-Identifier: GPL-2.0 */

ENTRY(_start)

PHDRS
{
	text PT_LOAD FLAGS(7);
}

SECTIONS
{
	.text : ALIGN(4) {
		*(.text)
		*(.text.*)
	} :text = 0

	.data : ALIGN(4) {
		*(.data)
		*(.data.*)
		*(.rodata)
		*(.rodata.*)
	} = 0

	.bss : ALIGN(4) {
		*(.bss)
		*(.bss.*)
		*(COMMON)
	}

	/DISCARD/ : {
		*(.comment)
		*(.note.*)
	}
}
