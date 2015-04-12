#ifndef _PANEL_H_
#define _PANEL_H_

#define RUN 1
#define STOP 2

#define RUN_CMD 0x80
#define SINGLE_STEP 0x40
#define EXAMINE 0x10
#define EXAMINE_NEXT 0x20
#define DEPOSIT 0x4
#define DEPOSIT_NEXT 0x8

void read_switches(uint16_t *address, uint8_t *cmd);
uint8_t sense_switches();

#endif
