/* SPDX-License-Identifier: GPL-2.0 */

OUTPUT_FORMAT("binary")

SECTIONS
{
	.text : ALIGN(4) {
		*(.text)
		*(.text.*)
	}

	.rodata : ALIGN(4) {
		*(.rodata)
		*(.rodata.*)
	}

	.data : ALIGN(4) {
		*(.data)
		*(.data.*)
	}

	.bss : ALIGN(4) {
		*(.bss)
		*(.bss.*)
	}

	/DISCARD/ : {
		*(.comment)
		*(.debug*)
		*(.note.*)
	}
}
