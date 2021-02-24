#ifndef STEPSEQUENCER_POTI_H /* Include guard */
#include <avr/io.h>

void init_poti_adc();
uint16_t adc_read_poti(uint8_t pin);
uint16_t get_poti_average(uint8_t pin);

#endif /* STEPSEQUENCER_POTI_H */