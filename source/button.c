#include "button.h"
#include "globals.h"

// Method sets all button pins as Input pins and activates internal pull up resistors for the inputs
void init_button_ports_mcu()
{
    BUTTON_REGISTER &= ~(1 << BUTTON_START_STOP) | ~(1 << BUTTON_1) | ~(1 << BUTTON_2) | ~(1 << BUTTON_3) | ~(1 << BUTTON_4);
    BUTTON_PORT = (1 << BUTTON_START_STOP) | (1 << BUTTON_1) | (1 << BUTTON_2) | (1 << BUTTON_3) | (1 << BUTTON_4);
}

// Method returns 1 if a button is pressed / 0 if button is not pressed
int button_pressed(uint8_t button, uint8_t opener_button)
{
    if (opener_button)
    {
        if ((BUTTON_PIN & (1 << button)))
        {
            return 1;
        }
    }
    else
    {
        if (!(BUTTON_PIN & (1 << button)))
        {
            return 1;
        }
    }
    return 0;
}