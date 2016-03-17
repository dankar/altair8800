#include <stdio.h>
#include <stdlib.h>
#include "intel8080.h"
#include "88dcdd.h"
#ifdef WIN32
	#include <Windows.h>
	#include <WinSock.h>
#else
	// socket
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <netinet/ip.h>
	#include <sys/ioctl.h>
	#include <fcntl.h>

	// strcat
	#include <string.h>
	#include "pi_panel.h"
	#include <wiringPi.h>
#endif

int sock;
int client_sock;

void dump_regs(intel8080_t *cpu)
{
	printf("%04x\t%02x\t%04x\tC: %02x\tD: %02x\tE: %02x\n", cpu->address_bus, cpu->data_bus, cpu->registers.pc, cpu->registers.c, cpu->registers.d, cpu->registers.e);
}

#ifdef WIN32
uint8_t key_states[256];

uint8_t get_key(uint8_t code)
{
	uint8_t ret_val = 0;

	if(GetAsyncKeyState(code))
	{
		if(!key_states[code])
			ret_val = 1;

		key_states[code] = 1;
	}
	else
	{
		key_states[code] = 0;
	}

	return ret_val;
}
#endif

uint8_t term_in()
{
	uint8_t b;

	if(recv(client_sock, (char*)&b, 1, 0) != 1)
	{
		return 0;
	}
	else
	{
		return b;
	}
}

void term_out(uint8_t b)
{
	b = b & 0x7f;
	send(client_sock, (char*)&b, 1, 0);
}

uint8_t memory[64*1024];
uint8_t cmd_switches;
uint16_t bus_switches;

void load_file(intel8080_t *cpu)
{
	size_t size = 0;
	FILE* fp = fopen("software/input.com", "rb");

	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	fread(&memory[0x100], 1, size, fp);
	fclose(fp);
}

const char *byte_to_binary(int x)
{
	int z;
    static char b[9];
    b[0] = '\0';

    for (z = 128; z > 0; z >>= 1)
    {
        strcat(b, ((x & z) == z) ? "1" : "0");
    }

    return b;
}


void load_mem_file(const char* filename, size_t offset)
{
	size_t size;
	FILE* fp = fopen(filename, "rb");
	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	fread(&memory[offset], 1, size, fp);
	fclose(fp);
}

uint8_t sense()
{
	return bus_switches >> 8;
}

void load_memory()
{
        load_mem_file("software/ROMs/DBL.bin", 0xff00);
        load_mem_file("software/ROMs/8K Basic/8kBas_e0.bin", 0xe000);
        load_mem_file("software/ROMs/8K Basic/8kBas_e8.bin", 0xe800);
        load_mem_file("software/ROMs/8K Basic/8kBas_f0.bin", 0xf000);
        load_mem_file("software/ROMs/8K Basic/8kBas_f8.bin", 0xf800);
}

int main(int argc, char *argv[])
{
	uint32_t counter = 0;
	unsigned long ok = 1;
	char yes = 1;
	struct sockaddr_in listen_addr;
	struct sockaddr client_addr;
	int sock_size;
	uint16_t breakpoint = 0x0;
	disk_controller_t disk_controller;
	intel8080_t cpu;

	rpi_init();

#ifdef WIN32
	WSADATA wsaData;

	if (WSAStartup(MAKEWORD(1, 1), &wsaData) != 0)
	{
		printf("Could not start WSA\n");
		return 1;
	}

	memset(key_states, 0, 256);

#endif

	memset(memory, 0, 64*1024);
	sock = socket(AF_INET, SOCK_STREAM, 0);

#ifdef WIN32
	ioctlsocket(sock, FIONBIO, &ok);
#endif

	setsockopt(sock, SOL_SOCKET, SO_LINGER, &yes, sizeof(char));
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

	listen_addr.sin_family = AF_INET;
	listen_addr.sin_addr.s_addr = INADDR_ANY;
	listen_addr.sin_port = htons(8800);
	memset(&(listen_addr.sin_zero), '\0', 8);

	if(bind(sock, (struct sockaddr*)&listen_addr, sizeof(listen_addr)) == -1)
	{
		printf("Could not bind\n");
	}

	printf("Waiting for terminal on port 8800...\n");

	do
	{
		listen(sock, 1);
		sock_size = sizeof(client_addr);
		client_sock = accept(sock, &client_addr, &sock_size);
	}while(client_sock == -1);

#ifndef WIN32
	fcntl(client_sock, F_SETFL, O_NONBLOCK);
#endif

	printf("Got connection. %d\n", client_sock);

	disk_controller.disk_function = disk_function;
	disk_controller.disk_select = disk_select;
	disk_controller.disk_status = disk_status;
	disk_controller.read = read;
	disk_controller.write = write;
	disk_controller.sector = sector;

	i8080_reset(&cpu, term_in, term_out, sense, &disk_controller);

	load_memory();

	// Mount diskette 1 (CP/M OS) and 2 (Tools)
	disk_drive.disk1.fp = fopen("software/CPM 2.2/cpm63k.dsk", "r+b");
	//disk_drive.disk1.fp = fopen("software/BASIC/Floppy Disk/Disk Basic Ver 300-5-F.dsk", "r+b");
	disk_drive.disk2.fp = fopen("software/CPM 2.2/games.dsk", "r+b");
	//disk_drive.disk2.fp = fopen("software/BASIC/Floppy Disk/Games on 300-5-F.dsk", "r+b");
	disk_drive.nodisk.status = 0xff;

	i8080_examine(&cpu, 0x0000); // ff00 loads from disk, e000 loads basic

	uint8_t cmd_state;
	uint8_t last_cmd_state = 0;
	uint8_t mode = STOP;
	uint32_t last_debounce = 0;

	uint32_t cycle_counter = 0;

	while(1)
	{
		if(mode == RUN)
		{
			i8080_cycle(&cpu);
			cycle_counter++;
			if(cycle_counter % 10 == 0)
				read_write_panel(0, cpu.data_bus, cpu.address_bus, &bus_switches, &cmd_switches, 1);
		}
		else
		{
			read_write_panel(0, cpu.data_bus, cpu.address_bus, &bus_switches, &cmd_switches, 1);
		}

		if(cmd_switches != last_cmd_state)
		{
			last_debounce = millis();
		}

		if((millis() - last_debounce) > 50)
		{
			if(cmd_switches != cmd_state)
			{
				cmd_state = cmd_switches;
				if(mode == STOP)
				{
					if(cmd_switches & STOP)
					{
						load_memory();
						i8080_examine(&cpu, 0);
					}
					if(cmd_switches & SINGLE_STEP)
					{
						i8080_cycle(&cpu);
					}
					if(cmd_switches & EXAMINE)
					{
						printf("Examine %x\n", bus_switches);
						i8080_examine(&cpu, bus_switches);
					}
					if(cmd_switches & EXAMINE_NEXT)
					{
						i8080_examine_next(&cpu);
					}
					if(cmd_switches & DEPOSIT)
					{
						i8080_deposit(&cpu, bus_switches & 0xff);
					}
					if(cmd_switches & DEPOSIT_NEXT)
					{
						i8080_deposit(&cpu, bus_switches & 0xff);
					}
					if(cmd_switches & RUN_CMD)
					{
						mode = RUN;
					}
				}
				if(mode == RUN)
				{
					if(cmd_switches & STOP_CMD)
					{
						mode = STOP;
					}
				}
			}
		}
		last_cmd_state = cmd_switches;
	}

	return 0;
}
