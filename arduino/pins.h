#ifndef _PINS_H_
#define _PINS_H_

/*
Purple = MOSI			(To 11)
Blue = LED output enable	(To 4)
Yellow = LED latch		(To 8)
Red = CLK			(To 13)

Purple (on memory) = CS		(To 5)
Yellow (on memory) = MISO	(To 12)

Green (on SD) = 3.3V
Grey (on SD) = CS		(To 6)

*/

#define LEDS_OE 4
#define LEDS_LATCH 8

#define MEMORY_CS 5
#define SD_CS 6

#endif
