#ifndef _88DCDD_H_
#define _88DCDD_H_

#include "types.h"

#define STATUS_ENWD			1
#define STATUS_MOVE_HEAD	2
#define STATUS_HEAD			4
#define STATUS_IE			32
#define STATUS_TRACK_0		64
#define STATUS_NRDA			128

#define CONTROL_STEP_IN		1
#define CONTROL_STEP_OUT	2
#define CONTROL_HEAD_LOAD	4
#define CONTROL_HEAD_UNLOAD 8
#define CONTROL_IE			16
#define CONTROL_ID			32
#define CONTROL_HCS			64
#define CONTROL_WE			128

#define SECTOR 137UL
#define TRACK (32UL*SECTOR)

typedef struct
{
#ifdef ARDUINO
	File fp;
#else
	FILE* fp;
#endif
	uint8_t track;
	uint8_t sector;
	uint8_t status;
	uint8_t write_status;
} disk_t;

typedef struct
{
	disk_t disk1;
	disk_t disk2;
	disk_t nodisk;
	disk_t *current;
} disks;

extern disks disk_drive;

void disk_select(uint8_t b);
uint8_t disk_status();
void disk_function(uint8_t b);
uint8_t sector();
void write(uint8_t b);
uint8_t read();


#endif
