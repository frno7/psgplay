// SPDX-License-Identifier: LGPL-2.1

#ifndef _FUJILIBC_ERRNO_H
#define _FUJILIBC_ERRNO_H

extern int errno;

#define __ERRNO_LIST(e)							\
	e(EOK,		 0, "Success")					\
	e(EPERM,	 1, "Operation not permitted")			\
	e(ENOENT,	 2, "No such file or directory")		\
	e(ESRCH,	 3, "No such process")				\
	e(EINTR,	 4, "Interrupted system call")			\
	e(EIO,		 5, "I/O error")				\
	e(ENXIO,	 6, "No such device or address")		\
	e(E2BIG,	 7, "Argument list too long")			\
	e(ENOEXEC,	 8, "Exec format error")			\
	e(EBADF,	 9, "Bad file number")				\
	e(ECHILD,	10, "No child processes")			\
	e(EAGAIN,	11, "Try again")				\
	e(ENOMEM,	12, "Out of memory")				\
	e(EACCES,	13, "Permission denied")			\
	e(EFAULT,	14, "Bad address")				\
	e(ENOTBLK,	15, "Block device required")			\
	e(EBUSY,	16, "Device or resource busy")			\
	e(EEXIST,	17, "File exists")				\
	e(EXDEV,	18, "Cross-device link")			\
	e(ENODEV,	19, "No such device")				\
	e(ENOTDIR,	20, "Not a directory")				\
	e(EISDIR,	21, "Is a directory")				\
	e(EINVAL,	22, "Invalid argument")				\
	e(ENFILE,	23, "File table overflow")			\
	e(EMFILE,	24, "Too many open files")			\
	e(ENOTTY,	25, "Not a typewriter")				\
	e(ETXTBSY,	26, "Text file busy")				\
	e(EFBIG,	27, "File too large")				\
	e(ENOSPC,	28, "No space left on device")			\
	e(ESPIPE,	29, "Illegal seek")				\
	e(EROFS,	30, "Read-only file system")			\
	e(EMLINK,	31, "Too many links")				\
	e(EPIPE,	32, "Broken pipe")				\
	e(EDOM,		33, "Math argument out of domain of func")	\
	e(ERANGE,	34, "Math result not representable")		\
	e(EBADRQC,	56, "Invalid request code")

enum {
#define __ERRNO_ENUM(symbol_, number_, description_)			\
	symbol_ = number_,
__ERRNO_LIST(__ERRNO_ENUM)
};

#endif /* _FUJILIBC_ERRNO_H */
