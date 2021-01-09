// SPDX-License-Identifier: LGPL-2.1
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#ifndef _TOSLIBC_TOS_SYSTEM_VARIABLE_H
#define _TOSLIBC_TOS_SYSTEM_VARIABLE_H

#include <stdint.h>

/**
 * struct conterm - Attribute bits for console sys
 * @kbshift: BCONIN returns shift status
 * @system_bell: enable system bell
 * @key_repeat: enable key repeat
 * @key_click: enable key-click
 */
struct conterm {
	uint8_t key_click : 1;
	uint8_t key_repeat : 1;
	uint8_t system_bell : 1;
	uint8_t kbshift : 1;
	uint8_t : 4;
};

#define SYSTEM_VARIABLE(v)											\
	v(0x400, 1, 4, evt_timer,   uint32_t,		"Timer handoff")					\
	v(0x404, 1, 4, evt_critic,  uint32_t,		"Critical error handoff")				\
	v(0x408, 1, 4, evt_term,    uint32_t,		"Process terminate handoff")				\
	v(0x40C, 5, 4, evt_xtra,    uint32_t,		"Reserved")						\
	v(0x420, 1, 4, memvalid,    uint32_t,		"0x752019F3 (memory controller valid)")			\
	v(0x424, 1, 2, memcntlr,    uint16_t,		"Memory controller configuration copy")			\
	v(0x426, 1, 4, resvalid,    uint32_t,		"0x31415926 to jump via reset vector 0x42a")		\
	v(0x42A, 1, 4, resvector,   uint32_t,		"Reset vector")						\
	v(0x42E, 1, 4, phystop,     uint32_t,		"Physical RAM top")					\
	v(0x432, 1, 4, _membot,     uint32_t,		"User memory begins")					\
	v(0x436, 1, 4, _memtop,     uint32_t,		"Upper end of user memory")				\
	v(0x43A, 1, 4, memval2,     uint32_t,		"0x237698AA together with memvalid")			\
	v(0x43E, 1, 2, flock,       uint16_t,		"Disk access, VBL disabled, if nonzero")		\
	v(0x440, 1, 2, seekrate,    uint16_t,		"0 - 6ms, 1 - 12ms, 2 - 2ms, 3 - 3ms (default)")	\
	v(0x442, 1, 2, _timr_ms,    uint16_t,		"System timer calibration")				\
	v(0x444, 1, 2, _fverify,    uint16_t,		"0 - no write verify else verify (default)")		\
	v(0x446, 1, 2, _bootdev,    uint16_t,		"System boot device number")				\
	v(0x448, 1, 2, palmode,     uint16_t,		"0 - NTSC (60Hz) else PAL (50Hz)")			\
	v(0x44A, 1, 2, defshftmd,   uint16_t,		"Default video res. if monitor changed")		\
	v(0x44C, 1, 2, sshiftmd,    uint16_t,		"Shadow shiftmd hardware register")			\
	v(0x44E, 1, 4, _v_bas_ad,   void *,		"Screen memory base pointer, 256 byte boundary")	\
	v(0x452, 1, 2, vblsem,      uint16_t,		"The VBL is not executed if zero")			\
	v(0x454, 1, 2, nvbls,       uint16_t,		"Number of vertical blank routines")			\
	v(0x456, 1, 4, _vblqueue,   void *,		"Vblank handlers pointer to routine address")		\
	v(0x45A, 1, 4, colorptr,    uint32_t,		"0 - null, else points to a colour palette")		\
	v(0x45E, 1, 4, screenpt,    void *,		"Pointer to screen base")				\
	v(0x462, 1, 4, _vbclock,    uint32_t,		"Counter for vblank interrupts")			\
	v(0x466, 1, 4, _frclock,    uint32_t,		"Counter for vblank interrupts")			\
	v(0x46A, 1, 4, hdv_init,    uint32_t,		"Hard disk initialise vector, else 0")			\
	v(0x46E, 1, 4, swv_vec,     uint32_t,		"Monitor changed vector")				\
	v(0x472, 1, 4, hdv_bpb,     uint32_t,		"Hard disk bpb vector, else 0")				\
	v(0x476, 1, 4, hdv_rw,      uint32_t,		"Hard disk read/write vector, else 0")			\
	v(0x47A, 1, 4, hdv_boot,    uint32_t,		"Hard disk boot vector, else 0")			\
	v(0x47E, 1, 4, hdv_mediach, uint32_t,		"Hard disk media change vector, else 0")		\
	v(0x482, 1, 2, _cmdload,    uint16_t,		"If nonzero execute COMMAND.PRG on boot")		\
	v(0x484, 1, 1, conterm,     struct conterm,	"Attribute bits for console sys")			\
	v(0x485, 1, 1, reserved,    uint8_t,		"Reserved")						\
	v(0x486, 1, 4, trp14ret,    uint32_t,		"Return address for the TRAP #14 call")			\
	v(0x48A, 1, 4, criticret,   uint32_t,		"Return address for evt_critic")			\
	v(0x48E, 4, 4, themd,       uint32_t,		"GEMDOS memory descriptor")				\
	v(0x49E, 1, 4, _md,         uint32_t,		"Additional memory descriptors")			\
	v(0x4A2, 1, 4, savptr,      void *,		"BIOS register save area pointer")			\
	v(0x4A6, 1, 2, _nflops,     uint16_t,		"Number of floppy disks attached (0, 1 or 2)")		\
	v(0x4A8, 1, 4, con_state,   uint32_t,		"State of conout() parser")				\
	v(0x4AC, 1, 2, save_row,    uint16_t,		"Save row number for x-y addressing")			\
	v(0x4AE, 1, 4, sav_contxt,  void *,		"Pointer to saved processor context")			\
	v(0x4B2, 2, 4, _bufl,       void *,		"GEMDOS two buffer-list pointers")			\
	v(0x4BA, 1, 4, _hz_200,     uint32_t,		"Counter for the 200 Hz clock")				\
	v(0x4BE, 1, 4, the_env,     uint32_t,		"Default environment string, four zero bytes")		\
	v(0x4C2, 1, 4, _drvbits,    uint32_t,		"32-bit vector for drives, 0 for A, 1 for B, etc.")	\
	v(0x4C6, 1, 4, _dskbufp,    void *,		"Pointer to common disk buffer, 1 KiB system BSS")	\
	v(0x4CA, 1, 4, _autopath,   void *,		"Pointer to autoexec path or null")			\
	v(0x4CE, 8, 4, _vbl_list,   uint32_t,		"List of standard VBL routines")			\
	v(0x4EE, 1, 2, _prt_cnt,    uint16_t,		"Initially -1, Alt_Help increments")			\
	v(0x4F0, 1, 2, _prtabt,     uint16_t,		"Printer abort flag dut to time-out")			\
	v(0x4F2, 1, 4, _sysbase,    void *,		"Pointer to start of the operating system")		\
	v(0x4F6, 1, 4, _shell_p,    void *,		"Global shell info pointer")				\
	v(0x4FA, 1, 4, end_os,      void *,		"Pointer to end of OS in RAM, start of the TPA")	\
	v(0x4FE, 1, 4, exec_os,     void *,		"Pointer to start of the AES")

#define SYSTEM_VARIABLE_1(count_, type_, label_) volatile type_ label_
#define SYSTEM_VARIABLE_N(count_, type_, label_) volatile type_ label_[count_]
#define SYSTEM_VARIABLE_2 SYSTEM_VARIABLE_N
#define SYSTEM_VARIABLE_4 SYSTEM_VARIABLE_N
#define SYSTEM_VARIABLE_5 SYSTEM_VARIABLE_N
#define SYSTEM_VARIABLE_8 SYSTEM_VARIABLE_N

struct system_variables {
#define SYSTEM_VARIABLE_STRUCT(						\
		address_, count_, size_, label_, type_, description_)	\
	SYSTEM_VARIABLE_##count_(count_, type_, label_);
SYSTEM_VARIABLE(SYSTEM_VARIABLE_STRUCT)
} __attribute__((__packed__));

#if defined(__m68k__)

extern struct system_variables *__system_variables;

#endif /* defined(__m68k__) */

#endif /* _TOSLIBC_TOS_SYSTEM_VARIABLE_H */
