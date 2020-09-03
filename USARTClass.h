#ifndef USARTClass_H_
#define USARTClass_H_

#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#ifndef BAUD
#define BAUD 9600
#endif

#define   USART_HAS_DATA   bit_is_set(UCSR0A, RXC0)
#define   USART_READY      bit_is_set(UCSR0A, UDRE0)

class USARTClass{
private:
public:
	void init();
	void transmitByte(uint8_t data);
	uint8_t receiveByte(void);
	void printString(const char myString[]); // Utility function to transmit an entire string from RAM
	void readString(char myString[], uint8_t maxLength); // Define a string variable, pass it to this function
                                                         //The string will contain whatever you typed over serial
	void printByte(uint8_t byte);// Prints a byte out as its 3-digit ascii equivalent
	void printWord(uint16_t word);/* Prints a word (16-bits) out as its 5-digit ascii equivalent */
	void printBinaryWord(uint16_t word);/* Prints two bytes out in 1s and 0s */
	void printBinaryByte(uint8_t byte);/* Prints a byte out in 1s and 0s */
	void printBinaryNibble(uint8_t Nibble);/* Prints a nibble out in 1s and 0s */
	char nibbleToHex(uint8_t nibble);
	char nibbleToHexCharacter(uint8_t nibble);
	void printHexByte(uint8_t byte);          /* Prints a byte out in hexadecimal */
	void printHexWord(uint16_t word);
	uint8_t getNumber();/* takes in up to three ascii digits,converts them to a byte when press enter */
	uint16_t getNumber16();
	void printFloat(float number);
	void printNibble(uint8_t nibble);
	void printROM(uint16_t address, uint16_t n, uint8_t *buff);
	void printString_Progmem(const char *strP);

	USARTClass();
};

extern USARTClass usart;

#endif