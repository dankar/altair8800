extern "C"
{
	#include "intel8080.h"
}

intel8080_t cpu;
disk_controller_t disk_controller;
uint32_t start_time = 0;
uint32_t end_time = 0;
uint32_t counter = 0;

void term_out(uint8_t c)
{
	Serial.write(c & 0x7f);
}

uint8_t term_in()
{
	Serial.write(0xc0);
	delay(20);
	if(Serial.available())
	{
		return Serial.read();
	}
	else
	{
		return 0;
	}
}

uint8_t read8(uint16_t address)
{
	Serial.write(0xee);
	Serial.write((uint8_t)(address >> 8));
	Serial.write((uint8_t)(address & 0xff));
	while(Serial.available() == 0);
	return Serial.read();
}

void write8(uint16_t address, uint8_t val)
{
	Serial.write(0xef);
        Serial.write((uint8_t)(address >> 8));
        Serial.write((uint8_t)(address & 0xff));
	Serial.write(val);
}

uint16_t read16(uint16_t address)
{
	uint16_t result = 0;
	Serial.write(0xec);
        Serial.write((uint8_t)(address >> 8));
        Serial.write((uint8_t)(address & 0xff));
	while(Serial.available() == 0);
	result = Serial.read() << 8;
	while(Serial.available() == 0);
	result |= Serial.read();

	return result;
}

void write16(uint16_t address, uint16_t val)
{
	Serial.write(0xed);
        Serial.write((uint8_t)(address >> 8));
        Serial.write((uint8_t)(address & 0xff));
	Serial.write((uint8_t)(val >> 8));
	Serial.write((uint8_t)(val & 0xff));
}

void setup()
{
	Serial.begin(2000000);
	memset(&disk_controller, 0, sizeof(disk_controller_t));
	i8080_reset(&cpu, term_in, term_out, read8, write8, read16, write16, &disk_controller);
	i8080_examine(&cpu, 0x00);
}

void loop()
{
	i8080_cycle(&cpu);
}
