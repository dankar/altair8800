altair8800
==========

What is this?
-------------

What started as a Intel 8080 emulator turned into a Altair 8800 emulator with associated hardware for a front panel.

An updated version of this project is available at https://github.com/dankar/nanoaltair8800

Building
--------

The build for Arduino Uno is the most up to date. It should work to run "./build" and then "make && upload" in the arduino folder to compile and program.

arduino-mk is used by the makefile.

The code will look for certain files on the SD card. "88dskrom.bin" is a disk bootloader which will be loaded to offset 0xff00 in memory. "disk1.bin" and "disk2.bin" will be used by the 88-DCDD emulator and will act as disk 1 and disk 2 respectively.

Completeness
------------

Passes Kelly Smith test and cputest.com. Runs CP/M and all kinds of BASIC I could find. Altair DOS not working for some reason (complains about insufficient memory). Support for 88-DCDD. No tape or cassette.

There are probably still a few bugs that needs ironing out.

The performance is pretty poor. The emulator runs a couple of times slower than an original Altair 8800 which is a bit disappointing. It's of course also possible porting it to a faster MCU with more memory. Higher optimization levels will cause the code not to fit on an atmega328 but there are still code optimizations to be done. It is currently not possible to stop execution once started with the RUN/STOP switch. It's way too slow to check the front panel switches for each main loop.

Hardware
--------

A front panel has been designed and is available as a KiCad project under the PCB folder. The front panel is basically a bunch of shift registers for input from the switches and output to the LEDs, as well as a 23LC1024 SPI SRAM chip for memory and a MicroSD reader. Everything is attached on the SPI bus to the Arduino.

Known bugs
----------

The current hardware implementation has a bug in the MicroSD card holder-area. The pins are connected as they should be for a regular size SD card and not a MicroSD. If you want to use the panel with a MicroSD card you need to fix this.

Raspberry Pi
------------

The current code in the repo now supports running on Raspberry Pi instead of Arduino. This means that the MicroSD slot is not necessary any more, as well as the SRAM chip. The code runs faster on a Raspberry Pi as well. I would recommend this route for anyone interested in their own panel.

Todo
----

* Separate the Intel 8080 emulator and the Altair 8800 hardware emulation. E.g. make the port IO more abstract instead of Altair 8800 specific.
* Optimizations
* RUN/STOP switch enabled when running
