altair8800
==========

What is this?
-------------

What started as a Intel 8080 emulator turned into a Altair 8800 emulator with associated hardware for a front panel.

Building
--------

The build for Arduino Uno is the most up to date. It should work to run "./build" and then "make && upload" in the arduino folder to compile and program.

arduino-mk is used by the makefile.

Completeness
------------

Passes Kelly Smith test and cputest.com. Runs CP/M and all kinds of BASIC I could find. Altair DOS not working for some reason (complains about insufficient memory). Support for 88-DCDD. No tape or cassette.

There are probably still a few bugs that needs ironing out.

The performance is pretty poor. The emulator runs a couple of times slower than an original Altair 8800 which is a bit disappointing. It's of course also possible porting it to a faster MCU with more memory. Higher optimization levels will cause the code not to fit on an atmega328 but there are still code optimizations to be done. It is currently not possible to stop execution once started with the RUN/STOP switch. It's way too slow to check the front panel switches for each main loop.

Hardware
--------

A front panel has been designed and is available as a KiCad project under the PCB folder. The front panel is basically a bunch of shift registers for input from the switches and output to the LEDs, as well as a 23LC1024 SPI SRAM chip for memory and a MicroSD reader. Everything is attached on the SPI bus to the Arduino.

