#ifndef _PI_PANEL_H_
#define _PI_PANEL_H_

#include "types.h"

void rpi_init();
void write_leds(uint8_t status, uint8_t data, uint16_t bus);

#endif
