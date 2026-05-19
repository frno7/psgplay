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
