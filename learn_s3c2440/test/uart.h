
#ifndef _UART_H
#define _UART_H

void uart_init(void);
int putchar(int c) ;
int getchar(void);
int puts(const char * s);
void hex_print(int n);
void print_space(void);

#endif



