#ifndef LOGGING_H_ /* Include guard */
#define LOGGING_H_

void init();
void uart_putchar(char c);
void usb_log(char *string);

#endif // LOGGING_H_
