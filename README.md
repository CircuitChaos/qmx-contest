# qmx-contest

QMX CW contest assistant and logger.

## Objective

QMX CW Contest Assistant is a program extending QMX transceiver with features useful in CW contesting.

Non-exhaustive list of features:

* Control VFO frequency from keyboard in 10 Hz and 250 Hz increments
* Control RF gain from keyboard
* Send preset and entered texts at given WPM
* Track contest exchanges (QSO numbers sent)
* Log QSOs to Cabrillo (CBR) file

Note that Cabrillo file generated by this program contains only QSO entries. You will have to edit it before submitting the log and add appropriate headers.

First working version was done in three evenings just before SPDX Contest. Further developments are possible.

## Building

First, install dependencies. On Debian-like systems:

`apt-get install git scons g++ libasound2 libasound-dev`

Then clone and build the repository. This one-liner should do the trick:

`git clone https://github.com/CircuitChaos/qmx-contest && cd qmx-contest && scons && sudo scons install`

If everything goes right, then program binary, called **qmx-contest**, will be installed in the */usr/local/bin* directory.

If you don't want to install it, you can skip the `sudo scons install` step and just use the binary built in *build* directory.

## CLI

Type `qmx-contest -h` to display command-line help. All switches are optional.

Note that program can be also used to just send CW from command line. For example:

`qmx-contest -t "vvv de circuit chaos" -w 20`

will send given text at 20 WPM and exit.

Example to use as a SPDX CW contest logger, for SP station with fixed exchange text („R”), in QSK mode (SP stations send only fixed exchange text, according to their voivodeship):

`qmx-contest -s SP5xxx -f spdx.cbr -P R -I ''`

Another example, for non-SP station starting with QSO #01 (so with minimum exchange text size of two characters), and not using QSK mode:

`qmx-contest -s aabccc -f spdx.cbr -I 01 -q`

## Text UI

Text UI is very simple and crude – that's one thing to rework. Generally you press keys and the program reacts. If you press `h`, you'll be presented with the help screen containing the same information as below.

### QMX control

* up / down: adjust gain
* left / right: adjust VFO frequency (10 Hz steps)
* pgup / pgdn: adjust VFO frequency (250 Hz steps)
* =: go to beginning of the band
* m: mute / unmute

### Built-in keyer

* u / d: adjust speed
* p: show presets
* 0...9: send preset
* t: send text
* a: abort sending

### Logging and contesting

* x: show current exchange
* l: log QSO (time is registered when pressed)

### Program control

* h: show help
* q: quit

## Limitations

Because of the way CW is generated, there's no envelope shaping. You will have clicks when sending this way. I didn't find any other way to generate CW via CAT on QMX.

## How CW is generated

In a hacky way.

A constant 1 kHz tone is sent to QMX on its audio interface. This tone doesn't seem to affect reception. It's used only when sending. Then, keydown and keyup events are generated internally, according to the Morse code to send and WPM. On keydown:

* QMX is switched to digital mode (it's assumed it was in CW mode)
* VFO frequency is decreased by 1 kHz
* TX is enabled

On keyup:

* TX is disabled
* VFO frequency is restored
* QMX is switched to CW mode

That's a lot of steps per symbol, so if it results in a jittery code being generated (it didn't in my tests), a `-q` option can be used to change this behavior. When using `-q`, preparation (mode switch and VFO adjustment) will be done only once, at the beginning of a string (before sending it), and restoration after the whole string finishes sending.

## TODO

Lots of things, really. This program is in a very early stage. Feel free to open PRs.

### High priority

* Key 'c' to check callsign (show CQ zone, ITU zone, check if it's in database of known callsigns, check if it's in log) – automatically load the log file if it's been edited by hand
* Support for cut numbers (so we can send A5 for CQ zone 15, but still have 15 in log)
* Band

### Rest

* Band limits for CW
* Band switch
* Frequency could be entered from keyboard
* Add panadapter (IQ mode, might be tricky if we still want to run Audacity together with the program)
* Exchange should be editable in UI
* Add preset sets (for example, separate sets for fox and hound modes)
* Investigate if it's possible to show that QMX entered SWR protection (some CAT command or something)
* UI interactions should be optionally logged to some file
* Add some thread to read incoming audio and display its average intensity over time (FFT waterfall is not really needed, something simpler should do), so it would be possible to decode Morse visually if it's too fast for the ears; might not be needed, as Audacity does great job as a recorder with preview
* Add incoming audio recording; probably not needed, see above comment about Audacity
* Rework text UI (keyboard handling is done in a quick, dirty, and possibly buggy way – use ncurses)
* Make sound handling more portable (now it uses raw ALSA, maybe portaudio would be better)
* Only VFO A is supported – detect VFO and adjust CAT commands instead
* Detect that a non-CW mode is selected and refuse operation
* Make presets configurable (from command-line, or a separate file)
* Allow sending other report than 599
* Allow detection of QMX CAT port by other VID and PID than default ones
* Add some reasonable limits to the gain
* Add some reasonable limits to the WPM
* Prevent frequency changes when sending
* Add frequency limits for the CW band (as an option)
* Allow frequency rollbacks (so if we go to 7000 kHz and try to decrement the frequency, we should roll back to 7040 kHz or something, not go to 6999 kHz – make it configurable) – or maybe make it different and just set minimum or maximum frequency then?
* Make keyer smarter – currently if text is being sent and another send is attempted, it's rejected, but it should be concatenated
* Seems that QMX finally supports sending CW text via CAT – use it
* Recalculate timeout in interrupted select() calls
* Smarter CLI handling (for example, empty exchange is now allowed – it shouldn't be)
* CAT receive buffer is not flushed properly before sending new command (there's code for it but it doesn't work, use util::watch() with zero timeout and read() in a loop instead)
* Handle partial writes (in CAT and elsewhere, maybe cmdPipe in the keyer too)
* There are two pipe pairs in the keyer, a single pair of connected sockets would be better (or cmdPipe replaced with a cond or something)
* Move all constants to one file (config.h or something)
* Allow remapping of keys
* Text entry is handled very poorly, basically the only way to clear entered text is Ctrl-U. Using readline() would be much better
* When sending text, show information about characters that couldn't be mapped to Morse
* Some smarter handling of a remote callsign – to set it as the current callsign (inserted into presets, checked against the log), or even: enter callsign, automatically send: "&lt;callsign&gt; 5NN &lt;xchg&gt;", and log this QSO
* Log editing (at least last entry)
* Add frequency presets, or at least one default frequency (set from CLI)
* Info about number of QSOs in log
* Info about bad command key pressed
* Reloading of log file
* Generally: log file submenu
* More Q-codes in presets (at least „QRL?”)
* Maybe add support for hamlib and other rigs via it
* Sort this TODO list, set priorities – right now it's chaotic
* Add some license

## Contact with the author

Please use the GitHub issue reporting system, or contact me at circuitchaos (at) interia (dot) com.
