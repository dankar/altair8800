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

	// windows typedefs
	typedef unsigned long DWORD;
	typedef unsigned short WORD;
	typedef unsigned int UNINT32;
#endif

int sock;
int client_sock;

#ifdef WIN32
void dump_regs(intel8080_t *cpu)
{
	char data[1024];

	sprintf(data, "%04x\t%02x\t%04x\n", cpu->address_bus, cpu->data_bus, cpu->registers.pc);
	OutputDebugStringA(data);
}
#endif

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
	//WriteConsoleA(console, &b, 1, &written, NULL);
}

/*port_out disk_select;
port_in	disk_status;
port_out disk_function;
port_in sector;
port_out write;
port_in read;*/

void load_file(intel8080_t *cpu)
{
	size_t size = 0;
	FILE* fp = fopen("software/input.com", "rb");

	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	fread(&cpu->memory[0x100], 1, size, fp);
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


int main(int argc, char *argv[])
{
	FILE* fp;
	size_t size;
	DWORD written;
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

#ifdef WIN32
	WSADATA wsaData;

	if (WSAStartup(MAKEWORD(1, 1), &wsaData) != 0)
	{
		printf("Could not start WSA\n");
		return 1;
	}

	memset(key_states, 0, 256);

#endif


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

	printf("Waiting for terminal...\n");
	
	do
	{
		listen(sock, 1);
		sock_size = sizeof(client_addr);
		client_sock = accept(sock, &client_addr, &sock_size);
	}while(client_sock == -1);

#ifndef WIN32
	fcntl(client_sock, F_SETFL, O_NONBLOCK);
#endif

	printf("Got connection? (%d)\n", client_sock);

	disk_controller.disk_function = disk_function;
	disk_controller.disk_select = disk_select;
	disk_controller.disk_status = disk_status;
	disk_controller.read = read;
	disk_controller.write = write;
	disk_controller.sector = sector;

	i8080_reset(&cpu, term_in, term_out, &disk_controller);

	// Load disk rom into memory
	fp = fopen("software/88dskrom.bin", "rb");
	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	fread(&cpu.memory[0xff00], 1, size, fp);
	fclose(fp);

	// Mount diskette 1 (CP/M OS) and 2 (Tools)
	disk_drive.disk1.fp = fopen("software/Cpm22.dsk", "r+b");
	disk_drive.disk2.fp = fopen("software/empty.dsk", "r+b");
	disk_drive.nodisk.status = 0xff;

	i8080_examine(&cpu, 0xff00);

	while(1)
	{
#ifdef WIN32
		if(get_key(VK_F1))
		{
			load_file(&cpu);
		}
#endif
		//if(cpu.registers.pc == breakpoint)
		//	__asm int 3;
		i8080_cycle(&cpu);
		i8080_sync(&cpu);
		//dump_regs(&cpu);
		//Sleep(1);
	}

	return 0;
}