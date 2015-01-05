#include <SPI.h>
#include <SdFat.h>
SdFat SD;

extern "C"
{
	#include "intel8080.h"
}

#include "88dcdd.h"

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

uint8_t read8(uint16_t address)
{
	uint8_t data;
	digitalWrite(5, LOW);
        SPI.transfer(3); // read byte
        SPI.transfer(0);
        SPI.transfer((address >> 8) & 0xff);
        SPI.transfer(address & 0xff); // 24 bit address
        data = SPI.transfer(0x00); // data
        digitalWrite(5, HIGH);
	return data;
}

void write8(uint16_t address, uint8_t val)
{
	digitalWrite(5, LOW);
        SPI.transfer(2); // write byte
        SPI.transfer(0);
        SPI.transfer((address >> 8) & 0xff);
        SPI.transfer(address & 0xff); // 24 bit address
        SPI.transfer(val); // data
        digitalWrite(5, HIGH);
}

uint16_t read16(uint16_t address)
{
	uint16_t result = 0;
	result = read8(address);
	result |= read8(address+1) << 8;

	return result;
}

void write16(uint16_t address, uint16_t val)
{
	write8(address, val & 0xff);
	write8(address+1, (val >> 8) & 0xff);
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

	pinMode(5, OUTPUT);
	digitalWrite(5, HIGH);

	SPI.setClockDivider(SPI_CLOCK_DIV2);
	
	if(!SD.begin(4))
	{
		Serial.println("SD");
		return;
	}

	digitalWrite(5, LOW);
	SPI.transfer(1); // Mode register
	SPI.transfer(0); // Byte mode
	digitalWrite(5, HIGH);

	for(int i = 0; i < 10; i++)
	{
		digitalWrite(5, LOW);
		SPI.transfer(2); // write byte
		SPI.transfer(0);
		SPI.transfer(0);
		SPI.transfer(i); // 24 bit address
		SPI.transfer(i); // data
		digitalWrite(5, HIGH);
	}


	for(int i = 0; i < 10; i++)
        {
                digitalWrite(5, LOW);
                SPI.transfer(3); // read byte
                SPI.transfer(0);
                SPI.transfer(0);
                SPI.transfer(i); // 24 bit address
                uint8_t data = SPI.transfer(0); // data
                digitalWrite(5, HIGH);

		if(i != data)
		{
			Serial.println("M");
			return;
		}
        }

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

	i8080_reset(&cpu, term_in, term_out, read8, write8, read16, write16, &disk_controller);
	i8080_examine(&cpu, 0xff00);

	Serial.println("OK!");
}

void loop()
{
	i8080_cycle(&cpu);

	SPI.transfer(0x00);
	SPI.transfer(0x00);
	SPI.transfer(0x00);
	SPI.transfer(0x00);
}
