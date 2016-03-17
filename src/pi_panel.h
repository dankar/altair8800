#ifndef _PI_PANEL_H_
#define _PI_PANEL_H_

#include "types.h"

#define RUN 1
#define STOP 2

#define STOP_CMD 0x02
#define RUN_CMD 0x01
#define SINGLE_STEP 0x08
#define EXAMINE 0x20
#define EXAMINE_NEXT 0x10
#define DEPOSIT 0x80
#define DEPOSIT_NEXT 0x40

void rpi_init();
void read_write_panel(uint8_t status, uint8_t data, uint16_t bus, uint16_t *bus_switches, uint8_t *cmd_switches, uint8_t write);

#endif
