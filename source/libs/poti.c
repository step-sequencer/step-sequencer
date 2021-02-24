#include "poti.h"

// Method initilaizes the analog digital conversion
void init_poti_adc()
{
    // Set the AREF input pin as a reference pin for ADC calculation
    ADMUX = (1 << REFS0);

    // ADC Enable and prescaler of 128 -> high prescaler -> low accuracy
    // F_CPU/128 = 125000
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

// Method reads analog input pin and returns value between 0 and 1023
uint16_t adc_read_poti(uint8_t pin)
{
    // Activate PC0 / ADC0 pin as analog input pin
    ADMUX = (ADMUX & 0xF8) | pin; // clears the bottom 3 bits before ORing

    // Setting ADSC bit will start conversion
    ADCSRA |= (1 << ADSC);

    // wait for conversion to complete
    // ADSC becomes ’0′ again
    while (ADCSRA & (1 << ADSC))
    {
    };

    // returning the result stored in the ADC register
    return (ADC);
}

// Method used to read poti values. It takes the average out of 64 reads
uint16_t get_poti_average(uint8_t pin)
{
    uint16_t potiSum = 0;
    for (int i = 0; i < 64; i++)
    {
        potiSum += adc_read_poti(pin);
    }
    return potiSum >> 6;
}