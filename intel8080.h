#ifndef _INTEL8080_H_
#define _INTEL8080_H_

#include <stdint.h>

#define FLAGS_CARRY		0x1
#define FLAGS_PARITY	0x4
#define FLAGS_H			16
#define FLAGS_IF		32
#define FLAGS_ZERO		64
#define FLAGS_SIGN		128

typedef struct 
{
	union
	{
		uint16_t af;

		struct {
			uint8_t flags;
			uint8_t a;
		};
	};

	union
	{
		uint16_t bc;
		struct
		{
			uint8_t c;
			uint8_t b;
			
		};
	};
	
	union
	{
		uint16_t de;
		struct
		{
			uint8_t e;
			uint8_t d;
			
		};
	};

	union
	{
		uint16_t hl;
		struct
		{
			uint8_t l;
			uint8_t h;
		};
	};

	uint16_t sp;
	uint16_t pc;
} registers_t;

typedef void (*port_out)(uint8_t b);
typedef uint8_t (*port_in)();

typedef struct
{
	port_out disk_select;
	port_in	disk_status;
	port_out disk_function;
	port_in sector;
	port_out write;
	port_in read;
} disk_controller_t;

typedef struct 
{
	uint8_t memory[64*1024];

	uint8_t data_bus;
	uint16_t address_bus;

	registers_t registers;

	uint8_t current_op_code;
	uint8_t decoder_step;
	uint16_t decoder_state;

	port_in term_in;
	port_out term_out;

	disk_controller_t disk_controller;
} intel8080_t;




void i8080_reset(intel8080_t *cpu, port_in in, port_out out, disk_controller_t *disk_controller);
void i8080_deposit(intel8080_t *cpu, uint8_t data);
void i8080_deposit_next(intel8080_t *cpu, uint8_t data);

void i8080_examine(intel8080_t *cpu, uint16_t address);
void i8080_examine_next(intel8080_t *cpu);

void i8080_cycle(intel8080_t *cpu);
void i8080_sync(intel8080_t *cpu);

#endif