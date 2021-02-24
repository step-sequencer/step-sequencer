#ifndef STEPSEQUENCER_BUTTON_H /* Include guard */
#include <avr/io.h>

void init_button_ports_mcu();
int button_pressed(uint8_t button, uint8_t pullup);

#endif /* STEPSEQUENCER_BUTTON_H */