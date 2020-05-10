// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Fredrik Noring
 */

#include <ctype.h>
#include <errno.h>
#include <signal.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "psgplay/print.h"

#include "system/unix/tty.h"

static struct termios tty_original;

static const struct tty_events *tty_events;

static void cbreak_mode(void)
{
	struct termios t;

	if (tcgetattr(STDIN_FILENO, &t) == -1)
		pr_fatal_errno("cbreak_mode:tcgetattr");

	tty_original = t;

	t.c_lflag &= ~(ICANON | ECHO);
	t.c_lflag |= ISIG;

	t.c_iflag &= ~ICRNL;

	t.c_cc[VMIN] = 1;
	t.c_cc[VTIME] = 0;

	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &t) == -1)
		pr_fatal_errno("cbreak_mode:tcsetattr");
}

static void tty_restore(void)
{
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &tty_original) == -1)
		pr_fatal_errno("tty_restore:tcsetattr");
}

static struct termios tty_save_restore(void)
{
	struct termios t;

	if (tty_events && tty_events->suspend)
		tty_events->suspend(tty_events->arg);

	if (tcgetattr(STDIN_FILENO, &t) == -1)
		pr_fatal_errno("tty_save_restore:tcgetattr");

	tty_restore();

	return t;
}

static void tty_reinstall(struct termios t)
{
	if (tcgetattr(STDIN_FILENO, &tty_original) == -1)
		pr_fatal_errno("tty_reinstall:tcgetattr");

	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &t) == -1)
		pr_fatal_errno("tty_reinstall:tcsetattr");

	if (tty_events && tty_events->resume)
		tty_events->resume(tty_events->arg);
}

static void handle_signal(int sig)
{
	if (tty_events && tty_events->suspend)
		tty_events->suspend(tty_events->arg);

	tty_restore();

	_exit(EXIT_SUCCESS);
}

static sigset_t unblock_signal_mask(int sig)
{
	sigset_t sig_mask;
	sigset_t old_mask;

	sigemptyset(&sig_mask);
	sigaddset(&sig_mask, sig);

	if (sigprocmask(SIG_UNBLOCK, &sig_mask, &old_mask) == -1)
		pr_fatal_errno("unblock_signal_mask:sigprocmask");

	return old_mask;
}

static void restore_signal_mask(sigset_t mask)
{
	if (sigprocmask(SIG_SETMASK, &mask, NULL) == -1)
		pr_fatal_errno("restore_signal_mask:sigprocmask");
}

static void install_signal(int sig, void (*handler)(int sig), int sa_flags)
{
	struct sigaction sa = {
		.sa_flags = sa_flags,
		.sa_handler = handler,
	};

	sigemptyset(&sa.sa_mask);

	if (sigaction(sig, &sa, NULL) == -1)
		pr_fatal_errno("install_signal:sigaction");
}

static void handle_stp(int sig)
{
	const int errno_ = errno;

	struct termios t = tty_save_restore();

	if (signal(SIGTSTP, SIG_DFL) == SIG_ERR)
		pr_fatal_errno("handle_stp:signal");

	raise(SIGTSTP);

	const sigset_t mask = unblock_signal_mask(SIGTSTP);

	/* Wait for SIGCONT */

	restore_signal_mask(mask);

	install_signal(SIGTSTP, handle_stp, SA_RESTART);

	tty_reinstall(t);

	errno = errno_;
}

struct tty_size tty_size(void)
{
	struct winsize ws;

	if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1)
		pr_fatal_errno("tty_size:ioctl");

	return (struct tty_size) {
		.rows = ws.ws_row,
		.cols = ws.ws_col,
	};
}

static void handle_winch(int sig)
{
	const int errno_ = errno;

	if (tty_events && tty_events->resize)
		tty_events->resize(tty_size(), tty_events->arg);

	errno = errno_;
}

bool tty_init(const struct tty_events *events)
{
	if (!isatty(STDIN_FILENO) ||
	    !isatty(STDOUT_FILENO))
		return false;

	tty_events = events;

	cbreak_mode();

	setbuf(stdout, NULL);

	install_signal(SIGTERM,  handle_signal, SA_RESTART);
	install_signal(SIGQUIT,  handle_signal, SA_RESTART);
	install_signal(SIGINT,   handle_signal, SA_RESTART);
	install_signal(SIGTSTP,  handle_stp,    SA_RESTART);
	install_signal(SIGWINCH, handle_winch,  0);

	return true;
}

void tty_exit(void)
{
	tty_restore();
}
