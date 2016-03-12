#include <wiringPi.h>
#include "pi_panel.h"
#include "pi_pins.h"

void rpi_init()
{
	wiringPiSetup();
        wiringPiSPISetup(0, 125000000);

        // Latch
        pinMode(LEDSTORE, OUTPUT);
        // OE
        pinMode(OE, OUTPUT);
        // MR
        pinMode(MR, OUTPUT);

        digitalWrite(MR, HIGH);

        // Output Enable
        digitalWrite(OE, LOW);

}

uint8_t reverse_lut[16] = {  0x0, 0x8, 0x4, 0xc, 0x2, 0xa, 0x6, 0xe, 0x1, 0x9, 0x5, 0xd, 0x3, 0xb, 0x7, 0xf };

void write_leds(uint8_t status, uint8_t data, uint16_t bus)
{
	uint32_t out;

	status = reverse_lut[(status & 0xf0) >> 4]  | reverse_lut[status & 0xf] << 4;
	data = reverse_lut[(data & 0xf0) >> 4] | reverse_lut[data & 0xf] << 4;

	bus = reverse_lut[(bus & 0xf000) >> 12] << 8 | reverse_lut[(bus & 0x0f00) >> 8] << 12 |  reverse_lut[(bus & 0xf0) >> 4] | reverse_lut[bus & 0xf] << 4;
	out = status << 24 | data << 16 | bus;

	digitalWrite(LEDSTORE, LOW);
	wiringPiSPIDataRW(0, &out, 4);
	digitalWrite(LEDSTORE, HIGH);
}
