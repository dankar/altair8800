#include <stdio.h>
#ifdef ARDUINO
	#include <Arduino.h>
	#include <SdFat.h>
#endif
#include "88dcdd.h"

//#define DISK_DEBUG

disks disk_drive;

void set_status(uint8_t bit)
{
	disk_drive.current->status &= ~bit;
}

void clear_status(uint8_t bit)
{
	disk_drive.current->status |= bit;
}


void disk_select(uint8_t b)
{
	uint8_t select = b & 0xf;

	if(select == 0)
	{
		disk_drive.current = &disk_drive.disk1;
	}
	else if(select == 1)
	{
		disk_drive.current = &disk_drive.disk2;
	}
	else
	{
		disk_drive.current = &disk_drive.nodisk;
	}
}

uint8_t disk_status()
{
#ifdef DISK_DEBUG
#ifdef ARDUINO
	Serial.print("Returning status ");
	Serial.print(disk_drive.current->status);
	Serial.println(" for disk");
#else
	printf("Returning status %d for disk\n", disk_drive.current->status);
#endif
#endif
	return disk_drive.current->status;
}

void disk_function(uint8_t b)
{
#ifdef DISK_DEBUG
#ifdef ARDUINO
	Serial.print("Disk function ");
	Serial.println(b);
#else
	printf("Disk function %d\n", b);
#endif
#endif

	if(b & CONTROL_STEP_IN)
	{
		disk_drive.current->track++;
		if(disk_drive.current->track != 0)
			clear_status(STATUS_TRACK_0);
#ifdef ARDUINO
		disk_drive.current->fp.seek(TRACK * disk_drive.current->track);
#else
		fseek(disk_drive.current->fp, TRACK * disk_drive.current->track, SEEK_SET);
#endif

#ifdef DISK_DEBUG
#ifdef ARDUINO
		Serial.print("Track seek to : ");
		Serial.println(TRACK * disk_drive.current->track);
#else
		printf("Track seek to : %d\n", TRACK * disk_drive.current->track);
#endif
#endif
	}
	if(b & CONTROL_STEP_OUT)
	{
		if(disk_drive.current->track > 0)
			disk_drive.current->track--;
		if(disk_drive.current->track == 0)
			set_status(STATUS_TRACK_0);
#ifdef ARDUINO
		disk_drive.current->fp.seek(TRACK * disk_drive.current->track);
#else
		fseek(disk_drive.current->fp, TRACK * disk_drive.current->track, SEEK_SET);
#endif
#ifdef DISK_DEBUG
#ifdef ARDUINO
                Serial.print("Track seek to : ");
                Serial.println(TRACK * disk_drive.current->track);
#else
                printf("Track seek to : %d\n", TRACK * disk_drive.current->track);
#endif
#endif
	}
	if(b & CONTROL_HEAD_LOAD)
	{
		set_status(STATUS_HEAD);
		set_status(STATUS_NRDA);
	}
	if(b & CONTROL_HEAD_UNLOAD)
	{
		clear_status(STATUS_HEAD);
	}
	if(b & CONTROL_IE)
	{
	}
	if(b & CONTROL_ID)
	{
	}
	if(b & CONTROL_HCS)
	{
	}
	if(b & CONTROL_WE)
	{
		set_status(STATUS_ENWD);
		disk_drive.current->write_status = 0;
	}
}

uint8_t sector()
{
	uint32_t seek;
	uint8_t ret_val;

	if(disk_drive.current->sector == 32)
	{
		disk_drive.current->sector = 0;
	}
	//current_sector = current_sector % 32;
	seek = disk_drive.current->track * TRACK + disk_drive.current->sector * (SECTOR);
#ifdef ARDUINO
	disk_drive.current->fp.seek(seek);
#else
	fseek(disk_drive.current->fp, seek, SEEK_SET);
#endif

	ret_val = disk_drive.current->sector << 1;
#ifdef DISK_DEBUG  
#ifdef ARDUINO
        Serial.print("Current sector: ");
	Serial.print(disk_drive.current->sector);
	Serial.print(" (");
	Serial.print(ret_val, HEX);
	Serial.print(") (bytes per track: ");
	Serial.print(TRACK);
	Serial.println(")");
#else
	printf("Current sector: %d (%X) (bytes per track: %d)\n", disk_drive.current->sector, ret_val, TRACK);
#endif
#endif

	disk_drive.current->sector++;
	return ret_val;
}

void write(uint8_t b)
{
#ifdef DISK_DEBUG
#ifdef ARDUINO
        Serial.print("Write ");
        Serial.print(b);
        Serial.print(" (byte in sector: ");
        Serial.print(disk_drive.current->write_status);
        Serial.println(")");
#else
	printf("Write %d (byte in sector: %d)\n", b, disk_drive.current->write_status);
#endif
#endif
#ifdef ARDUINO
	disk_drive.current->fp.write(&b, 1);
#else
	fwrite(&b, 1, 1, disk_drive.current->fp);
#endif
	if(disk_drive.current->write_status == 137)
	{
		disk_drive.current->write_status = 0;
		clear_status(STATUS_ENWD);
#ifdef DISK_DEBUG
#ifdef ARDUINO
		Serial.println("Disabling clear");
#else
		printf("Disabling clear\n");
#endif
#endif
	}
	else
		disk_drive.current->write_status++;
}

uint8_t read()
{
	static uint32_t bytes = 0;
	uint8_t b;

#ifdef ARDUINO
	b = disk_drive.current->fp.read();
#else
	fread(&b, 1, 1, disk_drive.current->fp);
#endif

	bytes++;

#ifdef DISK_DEBUG
#ifdef ARDUINO
        Serial.print("Reading byte ");
        Serial.print(bytes);
        Serial.print(" (");
        Serial.print(b, HEX);
        Serial.println(")");
#else
	printf("Reading byte %d (%x)\n", bytes, b);
#endif
#endif
	return b;
}
