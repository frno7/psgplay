![compilation workflow](https://github.com/frno7/psgplay/actions/workflows/compilation.yml/badge.svg)

![Atari ST loading screen](https://raw.githubusercontent.com/frno7/psgplay/main/doc/atari-load.png)
![Atari ST main menu](https://raw.githubusercontent.com/frno7/psgplay/main/doc/atari-main.png)

PSG play is a music player and emulator for the
[Atari ST](https://en.wikipedia.org/wiki/Atari_ST)
[Programmable Sound Generator](https://en.wikipedia.org/wiki/Programmable_sound_generator) (PSG)
[YM2149](https://en.wikipedia.org/wiki/General_Instrument_AY-3-8910)
and files in the [SNDH archive](http://sndh.atari.org/).

# How to download

- [Gentoo Linux](https://en.wikipedia.org/wiki/Gentoo_Linux) has a
  [`media-sound/psgplay`](https://github.com/frno7/gentoo.overlay/tree/main/media-sound/psgplay)
  package.
- [Arch Linux](https://en.wikipedia.org/wiki/Arch_Linux) has a
  [`psgplay-git`](https://aur.archlinux.org/packages/psgplay-git) package.

Github [actions](https://github.com/frno7/psgplay/actions/workflows/compilation.yml)
automatically compile and publish archives with PSG play for
the [Atari ST](https://en.wikipedia.org/wiki/Atari_ST),
as well as Linux and the architecture
[`x86-64`](https://en.wikipedia.org/wiki/X86-64), and a
[`wasm`](https://en.wikipedia.org/wiki/WebAssembly) web browser library
for use with [Cowbell](https://github.com/demozoo/cowbell). These are built with
[`.github/workflows/compilation.yml`](https://github.com/frno7/psgplay/blob/main/.github/workflows/compilation.yml).

# How to build

This repository has Git [submodules](https://git-scm.com/docs/git-submodule)
so clone it with the `--recurse-submodules` option, or do
`git submodule update --init --recursive`.

For Linux and Mac OS, do `make psgplay` to compile `psgplay`.
For BSD systems, such as FreeBSD, replace `make` with `gmake`. To use
[Advanced Linux Sound Architecture](https://en.wikipedia.org/wiki/Advanced_Linux_Sound_Architecture)
(ALSA) and _interactive text mode_, do `make ALSA=1 psgplay`. To use
[PortAudio](https://en.wikipedia.org/wiki/PortAudio) and
_interactive text mode_, do `make PORTAUDIO=1 psgplay`.

For Atari ST, do `make TARGET_COMPILE=m68k-elf- PSGPLAY.TOS`.

For [Javascript](https://en.wikipedia.org/wiki/JavaScript),
[Webassembly](https://en.wikipedia.org/wiki/WebAssembly), and the
[Emscripten](https://en.wikipedia.org/wiki/Emscripten) compiler, do
`make HOST_CC=emcc web`. The PSG play library is available with
[Cowbell](https://github.com/demozoo/cowbell), having a particular
focus on [demoscene](https://en.wikipedia.org/wiki/Demoscene) music.

The `BUILD_CC`, `HOST_AR`, `HOST_CC`, and `TARGET_CC` with `TARGET_LD`
[`Makefile`](https://github.com/frno7/psgplay/blob/main/Makefile)
variables can be configured for various compilation settings.
The `BUILD_CFLAGS`, `HOST_CFLAGS`, `TARGET_CFLAGS`, and `TARGET_LDFLAGS`
variables are available as well.

Type `make install` to install everything, by default in `~/.local/usr`.
Set `prefix` to change the directory, for example
`make prefix=$HOME/some/place/else install`. More specific subtargets than
`install` are also available, for instance `install-lib`, `install-psgplay`,
and so on. Set `DESTDIR` for
[staged installs](https://www.gnu.org/prep/standards/html_node/DESTDIR.html).


Review the file
[`INSTALL`](https://github.com/frno7/psgplay/blob/main/INSTALL)
for installation instructions.

# How to use

PSG play options for Linux and Mac OS:


```
Usage: psgplay [options]... <sndh-file>

General options:

    -h, --help             display this help and exit
    --version              display version and exit
    -v, --verbose          increase verbosity

    -i, --info             display SNDH file info and exit

Play options:

    -o, --output=<file>    write audio output to the given file in WAVE format
                           or to an ALSA handle if prefixed with "alsa:"

    --start=<[mm:]ss.ss>   start playing at the given time
    --stop=<[mm:]ss.ss|auto|never>
                           stop playing at the given time, or automatically
                           if the track has a known duration, or never
    --length=<[mm:]ss.ss>  play for the given duration

    -m, --mode=<command|text>
                           command or interactive text mode

    -t, --track=<num>      set track number
    -f, --frequency=<num>  set audio frequency in Hz (default 44100)

    --psg-mix=<empiric|linear>
                           empiric (default) mixes the three PSG channels as
                           measured on Atari ST hardware; linear sums the
                           channels to produce a cleaner sound
    --psg-balance=<A:B:C>  set balance between -1 (left) and +1 (right) for
                           PSG channels A, B and C. For example -0.4:0:+0.4
                           for stereo effect. Default is 0:0:0 for mono.
    --psg-volume=<A:B:C>   set volume between 0 (off) and +1 (max) for
                           PSG channels A, B and C. For example 0:0:1 to
                           play channel C only. Default is 1:1:1.

Disassembly options:

    --disassemble          disassemble SNDH file and exit; may be combined
                           with the --trace=cpu option for self-modifying code,
                           disassembly of interrupt code, etc.
    --disassemble-header   disassemble SNDH file header and exit
    --disassemble-address  display address column in disassembly
    --remake-header        remake SNDH file header in disassembly

Trace options:

    --trace=<device>,...   trace device operations of SNDH file and exit:
                           all cpu reg
```

## Interactive text mode

PSG play defaults to _interactive text mode_ if it is compiled with ALSA
or PortAudio for Linux or Mac OS, or is compiled for Atari ST. If no audio
output support is present, PSG play will default to
[WAVE](https://en.wikipedia.org/wiki/WAV) format output.

For Linux and Mac OS, [TTY](https://en.wikipedia.org/wiki/Tty_(Unix)) mode
and [ECMA-48](https://en.wikipedia.org/wiki/ANSI_escape_code) are used,
including support for job control such as process suspension.

For Atari ST, text mode and [VT52](https://en.wikipedia.org/wiki/VT52)
are used. See issues
[#5](https://github.com/frno7/psgplay/issues/5) and
[#6](https://github.com/frno7/psgplay/issues/6)
for ideas about additional
[serial port](https://en.wikipedia.org/wiki/Serial_port) and
[GEM](https://en.wikipedia.org/wiki/GEM_(desktop_environment))
user interfaces.

The currently playing tune is indicated in
[reverse video](https://en.wikipedia.org/wiki/Reverse_video).
A cursor is shown with `>`. Keyboard controls:

- `escape` or `q` to quit;
- `s` to stop;
- `p` or `spacebar` to pause;
- `1`, `2`, ..., `9` to play tunes 1, 2, ..., 9;
- `-` to decrease volume;
- `+` to increase volume;
- `<` to play the previous tune;
- `>` to play the next tune;
- `k` or `up arrow` to move the cursor up;
- `j` or `down arrow` to move the cursor down;
- `return` to play the tune at the cursor.

## Command mode

PSG play runs in _command mode_ if it is not compiled with ALSA for Linux,
or with PortAudio for Linux or Mac OS, or the options `-o`, `--output`, `--start`,
`--stop`, `--length`, `--disassemble` or `--trace` are given. Atari ST
does not support _command mode_.

## Improving performance

Most modern processors made during the last 20 years or so will easily
play in real-time, often using less than 10 % of processor resources.
However, they may struggle if other processes demand a lot of the system
at the same time.

On Linux, increasing the size of the ALSA buffer is the most effective
method to improve performance. _Interactive text mode_ uses a very small
buffer, 0.1 seconds, to remain responsive when using the keyboard.
_Command mode_ uses the default ALSA buffer size of the system, which may
(or may not) be significantly larger. Since _command mode_ is noninteractive,
audio latency is not a problem when the size of the buffer increases. One
approach is to double the duration of the system ALSA buffer until
performance problems disappear. Starting with, say, 5 seconds,
then 10 seconds, then 20 seconds, and so on. Try to configure the ALSA
parameter `buffer_time` (set in microseconds).

The [`nice`](https://en.wikipedia.org/wiki/Nice_(Unix)) command can also
be used to improve scheduling priority for PSG play.

## Library form

PSG play is compiled into the static library
`lib/psgplay/psgplay.a` and the shared library `lib/psgplay/psgplay.so`. The
[application programming interface](https://en.wikipedia.org/wiki/Application_programming_interface)
(API) is documented in
[`include/psgplay/psgplay.h`](https://github.com/frno7/psgplay/blob/main/include/psgplay/psgplay.h),
[`include/psgplay/stereo.h`](https://github.com/frno7/psgplay/blob/main/include/psgplay/stereo.h),
[`include/psgplay/sndh.h`](https://github.com/frno7/psgplay/blob/main/include/psgplay/sndh.h) and
[`include/psgplay/version.h`](https://github.com/frno7/psgplay/blob/main/include/psgplay/version.h).

The library also supplies an unaltered 250 kHz digital form for custom
analogue filters and mixers. This digital interface is documented in
[`include/psgplay/digital.h`](https://github.com/frno7/psgplay/blob/main/include/psgplay/digital.h).

There are two simple examples on how to use the PSG play library:

- [`lib/example/example-info.c`](https://github.com/frno7/psgplay/blob/main/lib/example/example-info.c)
  is an example on how to display SNDH tags;
- [`lib/example/example-play.c`](https://github.com/frno7/psgplay/blob/main/lib/example/example-play.c)
  is an example on how to play an SNDH file in 44.1 kHz stereo.

## Disassembly

PSG play can disassemble SNDH files. This can be used to debug, update
metadata and reassemble SNDH files. The `--disassemble` option can be used
for code inspection. The `--disassemble-header` option is the safest
choice when updating SNDH metadata, because most of the code is retained
with `dc.b` data bytes for exact reassembly.

The disassembly is guided by instruction reachability from the `init`,
`play`, and `exit` entry points, to separate executable instructions from
data. To deal with interrupt code and
[self-modifying code](https://en.wikipedia.org/wiki/Self-modifying_code),
use both the `--disassemble` and the `--trace=cpu` options. The disassembly
will then print what the processor actually executed in memory, which may
have been modified by the program itself, rather than the contents of the
SNDH file. The tracing execution length can be set with the `--length`
option.

The `--remake-header` option can be used to repair broken SNDH metadata such
as missing tags, excessive whitespace, etc. It can also be used to update or
add new metadata, by editing the produced assembly source code in an editor.

Excerpt of disassembly with the `--disassemble` option:

```
init:
	bra.w	_init				; init
exit:
	bra.w	_exit				; exit
play:
	bra.w	_play				; play
sndh:
	dc.b	$53,$4e,$44,$48,$43,$4f,$4d,$4d	; SNDHCOMM
	dc.b	$4d,$61,$64,$20,$4d,$61,$78,$00	; Mad Max.
	dc.b	$52,$49,$50,$50,$47,$72,$61,$7a	; RIPPGraz
	dc.b	$65,$79,$20,$2f,$20,$50,$48,$46	; ey / PHF
	dc.b	$00,$43,$4f,$4e,$56,$47,$72,$61	; .CONVGra
	dc.b	$7a,$65,$79,$20,$2f,$20,$50,$48	; zey / PH
	dc.b	$46,$00,$54,$49,$54,$4c,$57,$61	; F.TITLWa
	dc.b	$72,$70,$00,$23,$23,$30,$38,$00	; rp.##08.
	dc.b	$54,$43,$35,$30,$00,$00        	; TC50..
_init:
	lea	_9a(pc),a0			; init
	lea	_9e(pc),a1			; init
	move.l	a0,(a1)				; init
	subi.w	#1,d0				; init
	lea	_b4a(pc),a0			; init
	lea	_ac(pc),a1			; init
	move.l	d0,d1				; init
	asl.w	#3,d1				; init
	movea.l	(a1,d1.w),a2			; init
	addq.w	#6,d1				; init
	move.w	(a1,d1.w),d0			; init
	lea	_b4a(pc),a0			; init
	adda.l	a2,a0				; init
	lea	_ec(pc),a1			; init
	move.l	a0,56(a1)			; init
	move.l	a0,108(a1)			; init
	clr.w	2104(a1)			; init
	bsr.w	_ec				; init
	lea	_f4(pc),a0			; init
	lea	_9e(pc),a1			; init
	move.l	a0,(a1)				; init
_9a:
	rts					; init
_exit:
	rts					; exit
_9e:
	dc.b	$00,$00,$00,$00
_play:
	movea.l	_9e(pc),a0			; play
	jsr	(a0)				; play
	rts					; play
	...
```

Disassembly makes it possible to supply bug fixes and metadata updates
in source [patch](https://en.wikipedia.org/wiki/Patch_%28Unix%29) form,
for quick and easy review, application and SNDH file reassembly.

# Test and verification

Various technical aspects such as pitch, tempo, etc. can be tested and
verified with quality metrics and audio graphs. The tests are SNDH files
compiled from C, so an `m68k-elf` cross-compiler is required:

- `make -j TARGET_COMPILE=m68k-elf- test` compiles all tests.
- `make -j TARGET_COMPILE=m68k-elf- verify` compiles and verifies all tests.
- `make -j TARGET_COMPILE=m68k-elf- report` compiles and reports quality
   metrics for all tests.

More specific tests can be compiled, verified and reported. For example
`verify-psgpitch` to verify all `psgpitch` tests, or
`verify-psgpitch-3` to verify only the third test, and so on.

Making audio graph and report files:

- `make -j TARGET_COMPILE=m68k-elf- test/psgpitch-1.svg` compiles an SVG
  graph file.
- `make -j TARGET_COMPILE=m68k-elf- test/psgpitch-1.png` compiles a PNG
  graph file.
- `make -j TARGET_COMPILE=m68k-elf- test/psgpitch-1.report` compiles a
  report file.

Example report:

```
$ make -j TARGET_COMPILE=m68k-elf- report-psgpitch-2
path test/psgpitch-2.wave
name psgpitch
index 2
title PSG square wave C1 double low C 33 Hz
sample count 2686419 samples
sample duration 60.9 s
sample frequency 44100 Hz
tone clock 8010613 / 4 / 16 Hz
tone period 3793 cycles
wave reference frequency 32.999164 Hz
wave period 1336.398010 samples
wave frequency 32.999151 Hz
wave phase 42.000000 samples
wave zero crossing count 4021
wave zero crossing deviation max 0.910448 samples
wave error total count 1.022 samples
wave error total time 2.317e-05 s
wave error absolute frequency -0.000013 Hz
wave error relative frequency 3.80e-07
wave error relative tolerance 7.44e-07
```

Example graph:

![PSG pitch graph](https://raw.githubusercontent.com/frno7/psgplay/main/doc/psgpitch.png)

Set the `PSGPLAY_TEST` Makefile option to run the test suite with a command
other than `psgplay`, for example a command using [Hatari](https://hatari-emu.org/)
instead. Note that the first and last seconds are automatically cut from input
WAVE files during testing, to avoid artifacts due to audio fading, etc.

Review the file
[`INSTALL`](https://github.com/frno7/psgplay/blob/main/INSTALL)
for test and verification instructions.

# How it works

The [SNDH file format](https://github.com/frno7/psgplay/blob/main/doc/sndhv21.txt)
is an [Atari ST](https://en.wikipedia.org/wiki/Atari_ST)
machine code executable form of music. A substantial part of Atari ST
hardware must be emulated to play such files using other kinds of computers.
The five most complex parts emulated in software by PSG play are:

- the [Motorola 68000](https://en.wikipedia.org/wiki/Motorola_68000)
  processor, via the [Musashi](https://github.com/kstenerud/Musashi) library
  in [`lib/m68k`](https://github.com/frno7/psgplay/tree/main/lib/m68k);

- the [Programmable Sound Generator](https://en.wikipedia.org/wiki/Programmable_sound_generator)
  (PSG) [YM2149](https://en.wikipedia.org/wiki/General_Instrument_AY-3-8910)
  in [`lib/cf2149/module/cf2149.c`](https://github.com/frno7/cf2149/blob/main/module/cf2149.c);

- the [MC68901](https://archive.org/details/Motorola_MC68901_MFP_undated)
  multifunction peripheral (MFP) timer and interrupt controller in
  [`lib/cf68901/module/cf68901.c`](https://github.com/frno7/cf68901/blob/main/module/cf68901.c).

- the [DMA](https://en.wikipedia.org/wiki/Direct_memory_access) sound of the
  [Atari STE](https://en.wikipedia.org/wiki/Atari_ST#STE) in
  [`lib/cf300588/module/cf300588-sound.c`](https://github.com/frno7/cf300588/blob/main/module/cf300588-sound.c);

- the LMC1992 mixer of the [Atari STE](https://en.wikipedia.org/wiki/Atari_ST#STE_models)
  in [`lib/atari/mixer.c`](https://github.com/frno7/psgplay/tree/main/lib/atari/mixer.c);

The digital emulation is currently fairly accurate, aiming to be within the
variation of the compatible models of original Atari hardware. The analogue
emulation is currently simpler, aiming to be accurate but also avoid unwanted
artifacts such as the high level of noise produced with original Atari hardware.

The YM2149 PSG signal is unipolar, and has to be transformed to a bipolar
signal for mixing with stereo samples. To avoid sharp and audible noise when
starting and stopping playback, stereo samples fade in and out with a 10 ms
logistic sigmoid at start and stop.

As described in the issue [#9](https://github.com/frno7/psgplay/issues/9)
the LMC1992 for tone control specific to
[Atari STE](https://en.wikipedia.org/wiki/Atari_ST#STE_models)
is not yet fully emulated by PSG play.
