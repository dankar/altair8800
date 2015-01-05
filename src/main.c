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
		if(b == 10)
			b = 13;
		return b;
	}
}

void term_out(uint8_t b)
{
	b = b & 0x7f;
	send(client_sock, (char*)&b, 1, 0);
	//WriteConsoleA(console, &b, 1, &written, NULL);
}

/*port_out disk_select;
port_in	disk_status;
port_out disk_function;
port_in sector;
port_out write;
port_in read;*/

uint8_t memory[64*1024];

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


uint8_t read8(uint16_t address)
{
	if(address < 64*1024)
		return memory[address];
	return 0;
}

void write8(uint16_t address, uint8_t val)
{
	if(address < 64*1024)
		memory[address] = val;
}

uint16_t read16(uint16_t address)
{
	if(address < 64*1024)
		return *(uint16_t*)&memory[address];
	return 0;
}

void write16(uint16_t address, uint16_t val)
{
	if(address < 64*1024)
		*(uint16_t*)&memory[address] = val;
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

int main(int argc, char *argv[])
{
	FILE* fp;
	size_t size;
	unsigned int written;
	uint32_t counter = 0;
	unsigned long ok = 1;
	char yes = 1;
	struct sockaddr_in listen_addr;
	struct sockaddr client_addr;
	int sock_size;
	uint16_t breakpoint = 0x0;
	disk_controller_t disk_controller;
	intel8080_t cpu;
	int result;
	uint32_t test = 0;

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

	printf("Got connection.\n", client_sock);

	disk_controller.disk_function = disk_function;
	disk_controller.disk_select = disk_select;
	disk_controller.disk_status = disk_status;
	disk_controller.read = read;
	disk_controller.write = write;
	disk_controller.sector = sector;

	i8080_reset(&cpu, term_in, term_out, read8, write8, read16, write16, &disk_controller);

	
	load_mem_file("software/ROMs/DBL.bin", 0xff00);

	load_mem_file("software/ROMs/8K Basic/8kBas_e0.bin", 0xe000);
	load_mem_file("software/ROMs/8K Basic/8kBas_e8.bin", 0xe800);
	load_mem_file("software/ROMs/8K Basic/8kBas_f0.bin", 0xf000);
	load_mem_file("software/ROMs/8K Basic/8kBas_f8.bin", 0xf800);

	// Mount diskette 1 (CP/M OS) and 2 (Tools)
	disk_drive.disk1.fp = fopen("software/CPM 2.2/cpm63k.dsk", "r+b");
	//disk_drive.disk1.fp = fopen("software/BASIC/Floppy Disk/Disk Basic Ver 300-5-F.dsk", "r+b");
	disk_drive.disk2.fp = fopen("software/CPM 2.2/bdsc.dsk", "r+b");
	//disk_drive.disk2.fp = fopen("software/BASIC/Floppy Disk/Games on 300-5-F.dsk", "r+b");
	disk_drive.nodisk.status = 0xff;

	i8080_examine(&cpu, 0xff00); // ff00 loads from disk, e000 loads basic
	
	while(1)
	{
		//if(cpu.registers.pc == breakpoint)
		//	__asm int 3;
		i8080_cycle(&cpu);

		test++;

		if(test == 1000)
		{
			test = 0;
			//Sleep(1);
		}

		//dump_regs(&cpu);
	}

	return 0;
}
