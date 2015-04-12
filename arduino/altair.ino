#include <SPI.h>
#include <SdFat.h>
#include "intel8080.h"
#include "88dcdd.h"
#include "memory.h"
#include "pins.h"
#include "panel.h"

SdFat SD;

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
	if(Serial.available())
	{
		return Serial.read();
	}
	else
	{
		return 0;
	}
}

void load_to_mem(const char *filename, size_t offset)
{
	uint16_t counter = 0;
	File rom = SD.open(filename);

	if(!rom)
	{
		Serial.print("F:");
		Serial.println(filename);
		return;
	}

	rom.seek(0);

	while(rom.available())
	{
		write8(counter++ + offset, rom.read());
	}

	rom.close();
}

void setup()
{
	Serial.begin(115200);

	pinMode(SWITCHES_LOAD, OUTPUT);
	pinMode(SWITCHES_CS, OUTPUT);

	digitalWrite(SWITCHES_LOAD, HIGH);
	digitalWrite(SWITCHES_CS, HIGH);

	pinMode(MEMORY_CS, OUTPUT);
	digitalWrite(MEMORY_CS, HIGH);

	pinMode(LEDS_OE, OUTPUT);

	digitalWrite(LEDS_OE, LOW);

	pinMode(LEDS_LATCH, OUTPUT);
	digitalWrite(LEDS_LATCH, HIGH);

	if(!SD.begin(SD_CS, SPI_CLOCK_DIV2))
	{
		Serial.println("SD");
		return;
	}
	SPI.setClockDivider(SPI_CLOCK_DIV2);

	digitalWrite(MEMORY_CS, LOW);
	SPI.transfer(1); // Write mode register
	SPI.transfer(0); // Byte mode
	digitalWrite(MEMORY_CS, HIGH);

	load_to_mem("88dskrom.bin", 0xff00);

	disk_controller.disk_select = disk_select;
	disk_controller.disk_status = disk_status;
	disk_controller.disk_function = disk_function;
	disk_controller.sector = sector;
	disk_controller.write = write;
	disk_controller.read = read;

	memset(&disk_drive, 0, sizeof(disks));
	
	disk_drive.disk1.fp = SD.open("disk1.dsk", FILE_WRITE);
	disk_drive.disk2.fp = SD.open("disk2.dsk", FILE_WRITE);
	if(!disk_drive.disk2.fp || !disk_drive.disk1.fp)
		Serial.println("D");

	disk_drive.nodisk.status = 0xff;

	i8080_reset(&cpu, term_in, term_out, sense_switches, &disk_controller);
	i8080_examine(&cpu, 0x0000);
}

uint8_t mode = STOP; 

uint8_t cmd_state = 0;
uint8_t last_cmd_state = 0;
long last_debounce = 0;

void loop()
{
	uint16_t address;
	uint8_t cmd;

	if(mode == STOP)
	{
		read_switches(&address, &cmd);
	
		if(cmd != last_cmd_state)
		{
			last_debounce = millis();
		}
	
		if((millis() - last_debounce) > 50)
		{
			if(cmd != cmd_state)
			{
				cmd_state = cmd;
				if(cmd & SINGLE_STEP)
				{
					i8080_cycle(&cpu);
				}
				if(cmd & EXAMINE)
				{
					i8080_examine(&cpu, address);
				}
				if(cmd & EXAMINE_NEXT)
				{
					i8080_examine_next(&cpu);
				}
				if(cmd & DEPOSIT)
				{
					i8080_deposit(&cpu, address & 0xff);
				}
				if(cmd & DEPOSIT_NEXT)
				{
					i8080_deposit_next(&cpu, address & 0xff);
				}
				if(cmd & RUN_CMD)
				{
					mode = RUN;
				}
			}
		}
	
		last_cmd_state = cmd;
	}
	else
	{
		i8080_cycle(&cpu);
	}
}
