![Atari ST loading screen](https://raw.githubusercontent.com/frno7/psgplay/master/doc/atari-load.png)
![Atari ST main menu](https://raw.githubusercontent.com/frno7/psgplay/master/doc/atari-main.png)

PSG play is a music player and emulator for the
[Atari ST](https://en.wikipedia.org/wiki/Atari_ST)
[Programmable Sound Generator](https://en.wikipedia.org/wiki/Programmable_sound_generator) (PSG)
[YM2149](https://en.wikipedia.org/wiki/General_Instrument_AY-3-8910)
and files in the [SNDH archive](http://sndh.atari.org/).

# How to build

For Linux and Mac OS, do `make` to compile `psgplay`. To use
[Advanced Linux Sound Architecture](https://en.wikipedia.org/wiki/Advanced_Linux_Sound_Architecture)
(ALSA) and _interactive text mode_, do `make ALSA=1`.

For Atari ST, do `make CROSS_COMPILE=m68k-unknown-linux-gnu- PSGPLAY.TOS`.

Do `make help` to list further targets and options.

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

    --start=<[mm:]ss.ss>   start playing at the given time
    --stop=<[mm:]ss.ss|auto|never>
                           stop playing at the given time, or automatically
                           if the track has a known duration, or never
    --length=<[mm:]ss.ss>  play for the given duration

    -m, --mode=<command|text>
                           command or interactive text mode

    -t, --track=<num>      set track number
    -f, --frequency=<num>  set audio frequency in Hz (default 44100)

Disassembly options:

    --disassemble          disassemble SNDH file and exit
    --disassemble-header   disassemble SNDH file header and exit
    --remake-header        remake SNDH file header in disassembly
```

## Interactive text mode

PSG play defaults to _interactive text mode_ if it is compiled with ALSA
for Linux, or is compiled for Atari ST. Mac OS does not support
_interactive text mode_, only _command mode_ with
[WAVE](https://en.wikipedia.org/wiki/WAV) format output,
as described in issue [#8](https://github.com/frno7/psgplay/issues/8).

For Linux, [TTY](https://en.wikipedia.org/wiki/Tty_(Unix)) mode and
[ECMA-48](https://en.wikipedia.org/wiki/ANSI_escape_code) are used,
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
- `<` to play the previous tune;
- `>` to play the next tune;
- `k` or `up arrow` to move the cursor up;
- `j` or `down arrow` to move the cursor down;
- `return` to play the tune at the cursor.

## Command mode

PSG play runs in _command mode_ if it is not compiled with ALSA for Linux,
or is compiled for Mac OS, or the options `-o`, `--output`, `--start`,
`--stop` or `--length` are given. Atari ST does not support _command mode_.

## Library form

The PSG play shared library is compiled into `lib/psgplay/psgplay.so`. The
[application programming interface](https://en.wikipedia.org/wiki/Application_programming_interface)
(API) is documented in
[`include/psgplay/psgplay.h`](https://github.com/frno7/psgplay/blob/master/include/psgplay/psgplay.h),
[`include/psgplay/sndh.h`](https://github.com/frno7/psgplay/blob/master/include/psgplay/sndh.h) and
[`include/psgplay/version.h`](https://github.com/frno7/psgplay/blob/master/include/psgplay/version.h).

There are two simple examples on how to use the PSG play library:

- [`lib/psgplay/example-info.c`](https://github.com/frno7/psgplay/blob/master/lib/psgplay/example-info.c)
  is an example on how to display SNDH tags;
- [`lib/psgplay/example-play.c`](https://github.com/frno7/psgplay/blob/master/lib/psgplay/example-play.c)
  is an example on how to play an SNDH file in 44.1 kHz stereo.

Issue [#1](https://github.com/frno7/psgplay/issues/1) describes ideas
about making PSG play a linkable library.

## Disassembly

PSG play can disassemble SNDH files. This can be used to debug, update
metadata and reassemble SNDH files. The `--disassemble` option can be used
for code inspection. The `--disassemble-header` option is the safest
choice when updating SNDH metadata, because most of the code is retained
with `dc.b` data bytes for exact reassembly.

The `--remake-header` option can be used to repair broken SNDH metadata such
as missing tags, excessive whitespace, etc. It can also be used to update or
add new metadata, by editing the produced assembly source code in an editor.

See issue [#4](https://github.com/frno7/psgplay/issues/4) about switching
from MIT to Motorola syntax.

Excerpt of disassembly with the `--disassemble` option:

```
init:
	braw	_init
exit:
	braw	_exit
play:
	braw	_play
sndh:
	.dc.b	0x53,0x4e,0x44,0x48,0x43,0x4f,0x4d,0x4d /* SNDHCOMM */
	.dc.b	0x4d,0x61,0x64,0x20,0x4d,0x61,0x78,0x00 /* Mad Max. */
	.dc.b	0x52,0x49,0x50,0x50,0x47,0x72,0x61,0x7a /* RIPPGraz */
	.dc.b	0x65,0x79,0x20,0x2f,0x20,0x50,0x48,0x46 /* ey / PHF */
	.dc.b	0x00,0x43,0x4f,0x4e,0x56,0x47,0x72,0x61 /* .CONVGra */
	.dc.b	0x7a,0x65,0x79,0x20,0x2f,0x20,0x50,0x48 /* zey / PH */
	.dc.b	0x46,0x00,0x54,0x49,0x54,0x4c,0x57,0x61 /* F.TITLWa */
	.dc.b	0x72,0x70,0x00,0x23,0x23,0x30,0x38,0x00 /* rp.##08. */
	.dc.b	0x54,0x43,0x35,0x30,0x00,0x00           /* TC50.. */
_init:
	lea	%pc@(0x9a),%a0
	lea	%pc@(0x9e),%a1
	movel	%a0,%a1@
	subiw	#1,%d0
	lea	%pc@(0xb4a),%a0
	lea	%pc@(0xac),%a1
	movel	%d0,%d1
	aslw	#3,%d1
	moveal	%a1@(0,%d1:w),%a2
	addqw	#6,%d1
	movew	%a1@(0,%d1:w),%d0
	lea	%pc@(0xb4a),%a0
	addal	%a2,%a0
	lea	%pc@(_ec),%a1
	movel	%a0,%a1@(56)
	movel	%a0,%a1@(108)
	clrw	%a1@(2104)
	bsrw	_ec
	lea	%pc@(0xf4),%a0
	lea	%pc@(0x9e),%a1
	movel	%a0,%a1@
	rts
_exit:
	rts
	.dc.b	0x00,0x00,0x00,0x00
_play:
	moveal	%pc@(0x9e),%a0
	jsr	%a0@
	rts
	...
```

Excerpt of disassembly with the `--disassemble-header` option:

```
init:
	braw	_init
exit:
	braw	_exit
play:
	braw	_play
sndh:
	.dc.b	0x53,0x4e,0x44,0x48,0x43,0x4f,0x4d,0x4d /* SNDHCOMM */
	.dc.b	0x4d,0x61,0x64,0x20,0x4d,0x61,0x78,0x00 /* Mad Max. */
	.dc.b	0x52,0x49,0x50,0x50,0x47,0x72,0x61,0x7a /* RIPPGraz */
	.dc.b	0x65,0x79,0x20,0x2f,0x20,0x50,0x48,0x46 /* ey / PHF */
	.dc.b	0x00,0x43,0x4f,0x4e,0x56,0x47,0x72,0x61 /* .CONVGra */
	.dc.b	0x7a,0x65,0x79,0x20,0x2f,0x20,0x50,0x48 /* zey / PH */
	.dc.b	0x46,0x00,0x54,0x49,0x54,0x4c,0x57,0x61 /* F.TITLWa */
	.dc.b	0x72,0x70,0x00,0x23,0x23,0x30,0x38,0x00 /* rp.##08. */
	.dc.b	0x54,0x43,0x35,0x30,0x00,0x00           /* TC50.. */
_init:
	.dc.b	0x41,0xfa,0x00,0x46,0x43,0xfa,0x00,0x46
	.dc.b	0x22,0x88,0x04,0x40,0x00,0x01,0x41,0xfa
	.dc.b	0x0a,0xe8,0x43,0xfa,0x00,0x46,0x22,0x00
	.dc.b	0xe7,0x41,0x24,0x71,0x10,0x00,0x5c,0x41
	.dc.b	0x30,0x31,0x10,0x00,0x41,0xfa,0x0a,0xd2
	.dc.b	0xd1,0xca,0x43,0xfa,0x00,0x6e,0x23,0x48
	.dc.b	0x00,0x38,0x23,0x48,0x00,0x6c,0x42,0x69
	.dc.b	0x08,0x38,0x61,0x00,0x00,0x5e,0x41,0xfa
	.dc.b	0x00,0x62,0x43,0xfa,0x00,0x08,0x22,0x88
	.dc.b	0x4e,0x75
_exit:
	.dc.b	0x4e,0x75,0x00,0x00,0x00,0x00
_play:
	.dc.b	0x20,0x7a,0xff,0xfa,0x4e,0x90,0x4e,0x75
	...
```

Excerpt of disassembly with the `--disassemble-header` and
`--remake-header` options (having the missing `HDNS` tag automatically
repaired from the previous excerpt):

```
init:
	braw	_init
exit:
	braw	_exit
play:
	braw	_play
sndh:
	.ascii	"SNDH"
	.asciz	"COMMMad Max"
	.asciz	"RIPPGrazey / PHF"
	.asciz	"CONVGrazey / PHF"
	.asciz	"TITLWarp"
	.asciz	"##08"
	.asciz	"TC50"
	.even
	.ascii	"HDNS"
_init:
	.dc.b	0x41,0xfa,0x00,0x46,0x43,0xfa,0x00,0x46
	.dc.b	0x22,0x88,0x04,0x40,0x00,0x01,0x41,0xfa
	.dc.b	0x0a,0xe8,0x43,0xfa,0x00,0x46,0x22,0x00
	.dc.b	0xe7,0x41,0x24,0x71,0x10,0x00,0x5c,0x41
	.dc.b	0x30,0x31,0x10,0x00,0x41,0xfa,0x0a,0xd2
	.dc.b	0xd1,0xca,0x43,0xfa,0x00,0x6e,0x23,0x48
	.dc.b	0x00,0x38,0x23,0x48,0x00,0x6c,0x42,0x69
	.dc.b	0x08,0x38,0x61,0x00,0x00,0x5e,0x41,0xfa
	.dc.b	0x00,0x62,0x43,0xfa,0x00,0x08,0x22,0x88
	.dc.b	0x4e,0x75
_exit:
	.dc.b	0x4e,0x75,0x00,0x00,0x00,0x00
_play:
	.dc.b	0x20,0x7a,0xff,0xfa,0x4e,0x90,0x4e,0x75
	...
```

Disassembly makes it possible to supply bug fixes and metadata updates
in source [patch](https://en.wikipedia.org/wiki/Patch_%28Unix%29) form,
for quick and easy review, application and SNDH file reassembly:

```
--- sndh/Mad_Max/Games/Lethal_Xcess_(ST).S.orig	2020-05-22 14:56:20.495508523 +0200
+++ sndh/Mad_Max/Games/Lethal_Xcess_(ST).S.new	2020-05-22 15:23:55.260509689 +0200
@@ -6,13 +6,31 @@
 	braw	_play
 sndh:
 	.ascii	"SNDH"
-	.asciz	"TITLLethal Xcess (ST/Falc)"
-	.asciz	"COMMMad Max"
+	.asciz	"TITLLethal Xcess (ST)"
+	.asciz	"COMMJochen Hippel"
 	.asciz	"RIPPGrazey / PHF"
 	.asciz	"CONVGrazey / PHF"
+	.asciz	"YEAR1991"
 	.asciz	"TC50"
 	.asciz	"##07"
 	.even
+.subtitles:
+	.ascii	"!#SN"
+	.dc.w	.st1-.subtitles
+	.dc.w	.st2-.subtitles
+	.dc.w	.st3-.subtitles
+	.dc.w	.st4-.subtitles
+	.dc.w	.st5-.subtitles
+	.dc.w	.st6-.subtitles
+	.dc.w	.st7-.subtitles
+.st1:	.asciz	"Main Menu"
+.st2:	.asciz	"Level 1: Ruins of Methallycha 1"
+.st3:	.asciz	"Level 1: Ruins of Methallycha 2"
+.st4:	.asciz	"Level 2: Desert of No Return"
+.st5:	.asciz	"Level 3: The Evil Garden"
+.st6:	.asciz	"Level 4: Volcanic Plateaus"
+.st7:	.asciz	"Level 5: Fortress of Methallycha"
+	.even
 	.ascii	"HDNS"
 _init:
 	.dc.b	0x2f,0x00,0x41,0xfa,0x00,0x6e,0x4a,0x50
```

# How it works

The [SNDH file format](https://github.com/frno7/psgplay/blob/master/doc/sndhv21.txt)
is an [Atari ST](https://en.wikipedia.org/wiki/Atari_ST)
machine code executable form of music. A substantial part of Atari ST
hardware must be emulated to play such files using other kinds of computers.
The three most complex parts emulated in software by PSG play are:

- the [Motorola 68000](https://en.wikipedia.org/wiki/Motorola_68000)
  processor, via the [Musashi](https://github.com/kstenerud/Musashi) library
  in [`lib/m68k`](https://github.com/frno7/psgplay/tree/master/lib/m68k);

- the [Programmable Sound Generator](https://en.wikipedia.org/wiki/Programmable_sound_generator)
  (PSG) [YM2149](https://en.wikipedia.org/wiki/General_Instrument_AY-3-8910)
  in [`lib/atari/psg.c`](https://github.com/frno7/psgplay/tree/master/lib/atari/psg.c);

- the [MC68901](https://archive.org/details/Motorola_MC68901_MFP_undated)
  multifunction peripheral (MFP) timer and interrupt controller in
  [`lib/atari/mfp.c`](https://github.com/frno7/psgplay/tree/master/lib/atari/mfp.c).

The digital emulation is currently fairly accurate, aiming to be within the
variation of the compatible models of original Atari hardware. The analogue
emulation is currently simpler, aiming to be accurate but also avoid unwanted
artifacts such as the high level of noise produced with original Atari hardware.
As described in issue [#1](https://github.com/frno7/psgplay/issues/1),
a library form of PSG play could permit custom mixer and filter functions,
to obtain any level of analogue emulation.

As described in issues
[#9](https://github.com/frno7/psgplay/issues/9) and
[#10](https://github.com/frno7/psgplay/issues/10),
DMA sound and LMC1992 for tone and volume control specific to
[Atari STE](https://en.wikipedia.org/wiki/Atari_ST#STE_models)
and related hardware are not yet emulated by PSG play.
