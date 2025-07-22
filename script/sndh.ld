/* SPDX-License-Identifier: GPL-2.0 */

OUTPUT_FORMAT("binary")

SECTIONS
{
	.sndh 0 : {
		/* Generate bra.w targets, or rts in case of no identier. */
		SHORT (DEFINED(sndh_init) ? 0x6000         : 0x4e75);
		SHORT (DEFINED(sndh_init) ? sndh_init -  2 : 0x0000);
		SHORT (DEFINED(sndh_exit) ? 0x6000         : 0x4e75);
		SHORT (DEFINED(sndh_exit) ? sndh_exit -  6 : 0x0000);
		SHORT (DEFINED(sndh_play) ? 0x6000         : 0x4e75);
		SHORT (DEFINED(sndh_play) ? sndh_play - 10 : 0x0000);

		LONG (0x534e4448);	/* "SNDH" */
		KEEP (*(.sndh.title));
		. = ALIGN(2);
		KEEP (*(.sndh.tune_count));
		. = ALIGN(2);
		KEEP (*(.sndh.tune_name_tag));
		. = ALIGN(2);
		KEEP (*(.sndh.tune_name_offsets));
		. = ALIGN(2);
		KEEP (*(.sndh.tune_names));
		. = ALIGN(2);
		LONG (0x48444e53);	/* "HDNS" */
	}

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
