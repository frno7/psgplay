// SPDX-License-Identifier: LGPL-2.1

#ifndef _FUJILIBC_TOS_ERROR_H
#define _FUJILIBC_TOS_ERROR_H

#if defined(_TOSLIBC_SOURCE)

#define _TOS_ERRORS(E)							\
	/* BIOS */							\
	E(_OK,     0, EOK,     "Success")				\
	E(RROR,    1, EIO,     "Generic error")				\
	E(DRVNR,   2, EBUSY,   "Drive not ready")			\
	E(UNCMD,   3, EINVAL,  "Unknown command")			\
	E(_CRC,    4, EIO,     "CRC error")				\
	E(BADRQ,   5, EBADRQC, "Bad request")				\
	E(_SEEK,   6, ENXIO,   "Seek error")				\
	E(MEDIA,   7, ENXIO,   "Unknown media")				\
	E(SECNF,   8, EIO,     "Sector not found")			\
	E(PAPER,   9, ENOSPC,  "Out of paper")				\
	E(WRITF,  10, EIO,     "Write fault")				\
	E(READF,  11, EIO,     "Read fault")				\
	E(WRPRO,  12, EPERM,   "Device is write protected")		\
	E(_CHNG,  14, EIO,     "Media change detected")			\
	E(UNDEV,  15, ENODEV,  "Unknown device")			\
	E(BADSF,  16, EIO,     "Bad sectors on format")			\
	E(OTHER,  17, ENODEV,  "Insert other disk (request)")		\
	/* GEMDOS */   							\
	E(INVFN,  32, EINVAL,  "Invalid function")			\
	E(FILNF,  33, ENOENT,  "File not found")			\
	E(PTHNF,  34, ENOENT,  "Path not found")			\
	E(NHNDL,  35, EMFILE,  "No more handles")			\
	E(ACCDN,  36, EACCES,  "Access denied")				\
	E(IHNDL,  37, EINVAL,  "Invalid handle")			\
	E(NSMEM,  39, ENOMEM,  "Insufficient memory")			\
	E(IMBA,   40, EFAULT,  "Invalid memory block address")		\
	E(DRIVE,  46, ENODEV,  "Invalid drive specification")		\
	E(NSAME,  48, EXDEV,   "Cross device rename")			\
	E(NMFIL,  49, ENOENT,  "No more files")				\
	E(RANGE,  64, ERANGE,  "Range error")				\
	E(INTRN,  65, EFAULT,  "Internal error")			\
	E(PLFMT,  66, ENOEXEC, "Invalid program load format")		\
	E(GSBF,   67, ENOMEM,  "Memory block growth failure")

enum toserr {
#define _TOSERR_ENUM(identifier, number, errno, description)		\
	TOS_E##identifier = number,
	_TOS_ERRORS(_TOSERR_ENUM)
};

/**
 * errno_for_tos_error - kernel error number corresponding to a given TOS error
 * @toserr: TOS error number
 *
 * Return: approximative kernel error number
 */
int errno_for_tos_error(int toserr);

/**
 * tos_error_message - message corresponding to a given TOS error
 * @toserr: TOS error number
 *
 * Return: error message string
 */
const char *tos_error_message(int toserr);

#endif /* defined(_TOSLIBC_SOURCE) */

#endif /* _FUJILIBC_TOS_ERROR_H */
