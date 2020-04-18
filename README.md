`psgplay` is an YM2149 PSG music player for files in the
[SNDH archive](http://sndh.atari.org/).

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

    -t, --track=<num>      set track number
    -f, --frequency=<num>  set audio frequency in Hz (default 44100)
```
