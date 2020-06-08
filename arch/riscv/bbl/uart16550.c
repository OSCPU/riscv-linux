#define REG8(add)  *((volatile unsigned char *)  (add))
#define REG16(add) *((volatile unsigned short *) (add))
#define REG32(add) *((volatile unsigned int*)  (add))

#define UART_BASE 	0x41000000

/*****the uart register is little-endian and 4 byte-align******/

#define RB_THR_OFF  0x00 + 0x00
#define IER_OFF     0x04 + 0x00
#define IIR_FCR_OFF 0x08 + 0x00
#define LCR_OFF     0x0c + 0x00
#define MCR_OFF     0x10 + 0x00
#define LSR_OFF     0x14 + 0x00
#define MSR_OFF     0x18 + 0x00
#define CDRl_OFF    0x00 + 0x00
#define CDRh_OFF    0x04 + 0x01

void __am_uartlite_putchar(char ch) {
    if (ch == '\n') __am_uartlite_putchar('\r');

    while(!(REG8(UART_BASE+LSR_OFF) & 0x20));
    //wait until transmitter FIFO is empty
    //judge condition LSR[5]==1
    REG8(UART_BASE+RB_THR_OFF) = ch;
    
}

int __am_uartlite_getchar(void) {
  if (!(REG8(UART_BASE+LSR_OFF) & 0x1)) return -1;
  return REG8(UART_BASE+RB_THR_OFF);
}
