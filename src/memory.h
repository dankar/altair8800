#ifndef _MEMORY_H_
#define _MEMORY_H_

#include "types.h"

#ifdef ARDUINO

#include "../arduino/pins.h"

inline void led_out(uint16_t address, uint8_t data, uint8_t status)
{
	// This is needed since the data comes on the MISO line and we need it on the MOSI line.
	// TODO: Do something smart about it
	SPI.setBitOrder(LSBFIRST);
	digitalWrite(LEDS_LATCH, LOW);
        SPI.transfer(address & 0xff); // 24 bit address
	SPI.transfer((address >> 8) & 0xff);
	SPI.transfer(data);
	SPI.transfer(0x00); // Status LEDs
	digitalWrite(LEDS_LATCH, HIGH); // Disable LED shift registers
	SPI.setBitOrder(MSBFIRST);
}

inline uint8_t read8(uint16_t address)
{
        uint8_t data;
        digitalWrite(MEMORY_CS, LOW);

        SPI.transfer(3); // read byte
        SPI.transfer(0);
        SPI.transfer((address >> 8) & 0xff);
        SPI.transfer(address & 0xff); // 24 bit address
        data = SPI.transfer(0x00); // data
        digitalWrite(MEMORY_CS, HIGH);

	led_out(address, data, 0x00);

        return data;
}

inline void write8(uint16_t address, uint8_t val)
{
        digitalWrite(MEMORY_CS, LOW);

        SPI.transfer(2); // write byte
        SPI.transfer(0);
        SPI.transfer((address >> 8) & 0xff);
        SPI.transfer(address & 0xff); // 24 bit address
        SPI.transfer(val); // data
        digitalWrite(MEMORY_CS, HIGH);

	led_out(address, val, 0x00);
}

#else
#include "pi_panel.h"
extern uint8_t memory[64*1024];
extern uint8_t cmd_switches;
extern uint16_t bus_switches;

uint8_t read8(uint16_t address)
{
	uint8_t data;
        if(address < 64*1024)
                data =  memory[address];
	else
		data = 0;

        return data;
}

void write8(uint16_t address, uint8_t val)
{
        if(address < 64*1024)
                memory[address] = val;
}

#endif

inline uint16_t read16(uint16_t address)
{
        uint16_t result = 0;
        result = read8(address);
        result |= read8(address+1) << 8;

        return result;
}

inline void write16(uint16_t address, uint16_t val)
{
        write8(address, val & 0xff);
        write8(address+1, (val >> 8) & 0xff);
}

#endif
