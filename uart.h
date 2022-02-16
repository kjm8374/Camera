typedef unsigned char BYTE;
/**
 * Initialize the UART
 */
void uart0_init(void);

/**
 * Read a byte from the UART Rx
 * @return the character recieved
 */
BYTE uart0_getchar(void);

/**
 * Write a character to the UART Tx
 * @param ch character write
 */
void uart0_putchar(char ch);

/**
 * Write a standard C string to the UART Tx line
 * @param ptr_str string to write
 */
void uart0_put(char* ptr_str);
//trying to see if gets rid of no new line warning
