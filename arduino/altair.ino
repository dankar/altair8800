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

File memory;

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
	memory.seek(address);
	return memory.read();
}

void write8(uint16_t address, uint8_t val)
{
	memory.seek(address);
	memory.write(val);
}

uint16_t read16(uint16_t address)
{
	uint16_t result = 0;
	memory.seek(address);
	result = memory.read();
	result |= memory.read() << 8;

	return result;
}

void write16(uint16_t address, uint16_t val)
{
	memory.seek(address);
	memory.write((uint8_t)(val & 0xff));
	memory.write((uint8_t)(val >> 8));
}

File rom;
uint8_t buffer[512];

void load_to_mem(const char *filename, size_t offset)
{
	rom = SD.open(filename);

	if(!rom)
	{
		Serial.print("F:");
		Serial.println(filename);
		return;
	}

	rom.seek(0);
	memory.seek(offset);

	while(rom.available())
	{
		int i;
		for(i = 0; i < 512; i++)
		{
			if(rom.available())
				buffer[i] = rom.read();
			else
				break;
		}

		memory.write(buffer, i);
	}

	rom.close();
}

void clear_mem()
{
	
	memset(buffer, 0, 512);

	for(int i = 0; i < 128; i++)
	{
		memory.write(buffer, 512);
	}
}
		

void setup()
{
	Serial.begin(115200);

	if(!SD.begin(4))
	{
		Serial.println("SD");
		return;
	}

	//SPI.setClockDivider(SPI_CLOCK_DIV2);

	memory = SD.open("memory", FILE_WRITE);
	if(!memory)
	{
		Serial.println("MEM");
		return;
	}

	memory.seek(0);
	clear_mem();

	load_to_mem("4kbas32.bin", 0x0000);
	load_to_mem("88dskrom.bin", 0xff00);

	disk_controller.disk_select = disk_select;
	disk_controller.disk_status = disk_status;
	disk_controller.disk_function = disk_function;
	disk_controller.sector = sector;
	disk_controller.write = write;
	disk_controller.read = read;

	memset(&disk_drive, 0, sizeof(disks));
	
	disk_drive.disk1.fp = SD.open("disk1.dsk", FILE_WRITE);
	if(!disk_drive.disk1.fp)
		Serial.println("D1");	
	/*disk_drive.disk2.fp = SD.open("disk2.dsk", FILE_WRITE);
	if(!disk_drive.disk2.fp)
		Serial.println("No disk 2");*/

	disk_drive.disk2.status = 0xff;
	disk_drive.nodisk.status = 0xff;

	i8080_reset(&cpu, term_in, term_out, read8, write8, read16, write16, &disk_controller);
	i8080_examine(&cpu, 0xff00);

	Serial.println("OK!");
}

char spi_transfer(volatile char data)
{
	SPDR = data;
	while(!(SPSR & (1 << SPIF)));
	return SPDR;
}

void loop()
{
	i8080_cycle(&cpu);

	digitalWrite(5, LOW);
	spi_transfer((uint8_t)(cpu.address_bus >> 8));
	spi_transfer((uint8_t)(cpu.address_bus & 0xff));
	spi_transfer(cpu.data_bus);
	digitalWrite(5, HIGH);
}
