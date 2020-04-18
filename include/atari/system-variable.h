// SPDX-License-Identifier: GPL-2.0

#ifndef ATARI_SYSTEM_VARIABLE_H
#define ATARI_SYSTEM_VARIABLE_H

#include "psgplay/types.h"

#define SYSTEM_VARIABLE(v)						\
	v(0x400, 1, 4, evt_timer,   "Timer handoff")			\
	v(0x404, 1, 4, evt_critic,  "Critical error handoff")		\
	v(0x408, 1, 4, evt_term,    "Process terminate handoff")	\
	v(0x40C, 5, 4, evt_xtra,    "Reserved")				\
	v(0x420, 1, 4, memvalid,    "0x752019F3 (memory controller valid)")\
	v(0x424, 1, 2, memcntlr,    "Memory controller configuration copy")\
	v(0x426, 1, 4, resvalid,    "0x31415926 to jump via reset vector 0x42a")\
	v(0x42A, 1, 4, resvector,   "Reset vector")			\
	v(0x42E, 1, 4, phystop,     "Physical RAM top")			\
	v(0x432, 1, 4, _membot,     "User memory begins")		\
	v(0x436, 1, 4, _memtop,     "Upper end of user memory")		\
	v(0x43A, 1, 4, memval2,     "0x237698AA together with memvalid")\
	v(0x43E, 1, 2, flock,       "Disk access, VBL disabled, if nonzero")\
	v(0x440, 1, 2, seekrate,    "0 - 6ms, 1 - 12ms, 2 - 2ms, 3 - 3ms (default)")\
	v(0x442, 1, 2, _timr_ms,    "System timer calibration")		\
	v(0x444, 1, 2, _fverify,    "0 - no write verify else verify (default)")\
	v(0x446, 1, 2, _bootdev,    "System boot device number")	\
	v(0x448, 1, 2, palmode,     "0 - NTSC (60Hz) else PAL (50Hz)")	\
	v(0x44A, 1, 1, defshftmd,   "Default video res. if monitor changed")\
	v(0x44C, 1, 1, sshiftmd,    "Shadow shiftmd hardware register")	\
	v(0x44E, 1, 4, _v_bas_ad,   "Screen memory base pointer, 256 byte boundary")\
	v(0x452, 1, 2, vblsem,      "The VBL is not executed if zero")	\
	v(0x454, 1, 2, nvbls,       "Number of vertical blank routines")\
	v(0x456, 1, 4, _vblqueue,   "Vblank handlers pointer to routine address")\
	v(0x45A, 1, 4, colorptr,    "0 - null, else points to a colour palette")\
	v(0x45E, 1, 4, screenpt,    "Pointer to screen base")		\
	v(0x462, 1, 4, _vbclock,    "Counter for vblank interrupts")	\
	v(0x466, 1, 4, _frclock,    "Counter for vblank interrupts")	\
	v(0x46A, 1, 4, hdv_init,    "Hard disk initialise vector, else 0")\
	v(0x46E, 1, 4, swv_vec,     "Monitor changed vector")		\
	v(0x472, 1, 4, hdv_bpb,     "Hard disk bpb vector, else 0")	\
	v(0x476, 1, 4, hdv_rw,      "Hard disk read/write vector, else 0")\
	v(0x47A, 1, 4, hdv_boot,    "Hard disk boot vector, else 0")	\
	v(0x47E, 1, 4, hdv_mediach, "Hard disk media change vector else, 0")\
	v(0x482, 1, 2, _cmdload,    "If nonzero execute COMMAND.PRG on boot")\
	v(0x484, 1, 2, conterm,     "Attribute bits for console sys")	\
	v(0x485, 1, 1, reserved,    "Reserved,   ")			\
	v(0x486, 1, 4, trp14ret,    "Return address for the TRAP #14 call")\
	v(0x48A, 1, 4, criticret,   "Return address for evt_critic")	\
	v(0x48E, 1, 4, themd,       "GEMDOS memory descriptor")		\
	v(0x49E, 1, 2, _md,         "Additional memory descriptors")	\
	v(0x4A2, 1, 4, savptr,      "BIOS register save area pointer")	\
	v(0x4A6, 1, 2, _nflops,     "Number of floppy disks attached (0, 1 or 2)")\
	v(0x4A8, 1, 4, con_state,   "State of conout() parser")		\
	v(0x4AC, 1, 2, save_row,    "Save row number for x-y addressing")\
	v(0x4AE, 1, 4, sav_contxt,  "Pointer to saved processor context")\
	v(0x4B2, 1, 4, _buf1,       "GEMDOS two buffer-list pointers")	\
	v(0x4BA, 1, 4, _hz_200,     "Counter for the 200 Hz clock")	\
	v(0x4BE, 1, 4, the_env,     "Default environment string, four zero bytes")\
	v(0x4C2, 1, 4, _drvbits,    "32-bit vector for drives, 0 for A, 1 for B, etc.")\
	v(0x4C6, 1, 4, _dskbufp,    "Pointer to common disk buffer, 1 KiB system BSS")\
	v(0x4CA, 1, 4, _autopath,   "Pointer to autoexec path or null")	\
	v(0x4CE, 8, 4, _vbl_list,   "List of standard VBL routines")	\
	v(0x4EE, 1, 2, _prt_cnt,    "Initially -1, Alt_Help increments")\
	v(0x4F0, 1, 2, _prtabt,     "Printer abort flag dut to time-out")\
	v(0x4F2, 1, 4, _sysbase,    "Pointer to start of the operating system")\
	v(0x4F6, 1, 4, _shell_p,    "Global shell info pointer")	\
	v(0x4FA, 1, 4, end_os,      "Pointer to end of OS in RAM, start of the TPA")\
	v(0x4FE, 1, 4, exec_os,     "Pointer to start of the AES")

const char *system_variable_label(u32 address);

#endif /* ATARI_SYSTEM_VARIABLE_H */
