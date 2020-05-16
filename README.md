PSG play is an [Atari ST](https://en.wikipedia.org/wiki/Atari_ST)
[Programmable Sound Generator](https://en.wikipedia.org/wiki/Programmable_sound_generator) (PSG)
[YM2149](https://en.wikipedia.org/wiki/General_Instrument_AY-3-8910) music
player for files in the [SNDH archive](http://sndh.atari.org/).

Do `make` to compile `psgplay` for Linux or Mac OS. To use
[Advanced Linux Sound Architecture](https://en.wikipedia.org/wiki/Advanced_Linux_Sound_Architecture)
(ALSA), do `make ALSA=1`. Do `make help` to list further targets and options.

```
Usage: psgplay [options]... <sndh-file>

General options:

    -h, --help             display this help and exit
    --version              display version and exit
    -v, --verbose          increase verbosity

    -i, --info             display SNDH file info and exit
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
```
