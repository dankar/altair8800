#include <SPI.h>
#include "pins.h"
#include "panel.h"

void read_switches(uint16_t *address, uint8_t *cmd)
{
        uint16_t byte = 0;
        uint8_t cmd_result;

        digitalWrite(SWITCHES_CS, LOW);
        digitalWrite(SWITCHES_LOAD, LOW);
        delayMicroseconds(5);
        digitalWrite(SWITCHES_LOAD, HIGH);

        SPI.setBitOrder(LSBFIRST);
        *address = SPI.transfer(0x00);
        byte = SPI.transfer(0x00);
        *address |= byte << 8;
        *address = ~*address;

        *cmd = SPI.transfer(0x00);

        SPI.setBitOrder(MSBFIRST);
        digitalWrite(SWITCHES_CS, HIGH);
}

uint8_t sense_switches()
{
	uint16_t address;
	uint8_t cmd;

	read_switches(&address, &cmd);

	return address >> 8;
}
