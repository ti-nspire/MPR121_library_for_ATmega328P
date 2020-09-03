#include <avr/io.h>
#include "USARTClass.h"
#include <util/setbaud.h>
#include <math.h>
#include <avr/pgmspace.h>

void USARTClass::init() {                                /* requires BAUD */
  UBRR0H = UBRRH_VALUE;                        /* defined in setbaud.h */
  UBRR0L = UBRRL_VALUE;
#if USE_2X
  UCSR0A |= (1 << U2X0);
#else
  UCSR0A &= ~(1 << U2X0);
#endif
                                  /* Enable USART transmitter/receiver */
  UCSR0B = (1 << TXEN0) | (1 << RXEN0);
  UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);   /* 8 data bits, 1 stop bit */
}


void USARTClass::transmitByte(uint8_t data) {
                                     /* Wait for empty transmit buffer */
  loop_until_bit_is_set(UCSR0A, UDRE0);
  UDR0 = data;                                            /* send data */
}

uint8_t USARTClass::receiveByte() {
  loop_until_bit_is_set(UCSR0A, RXC0);       /* Wait for incoming data */
  return UDR0;                                /* return register value */
}


                       /* Here are a bunch of useful printing commands */
/*
void printString(const char myString[]) {
  uint8_t i = 0;
  while (myString[i]) {
    transmitByte(myString[i]);
    i++;
  }
}
*/
void USARTClass::printString(const char *strP){
	char oneLetter;
	while((oneLetter = *strP)){ // 代入であることを明示するため2重カッコにする。
		transmitByte(oneLetter);
		strP++;
	}
}





void USARTClass::readString(char myString[], uint8_t maxLength) {
  char response;
  uint8_t i;
  i = 0;
  while (i < (maxLength - 1)) {                   /* prevent over-runs */
    response = receiveByte();
    transmitByte(response);                                    /* echo */
    if (response == '\n') {                     /* enter marks the end */
      break;
    }
    else {
      myString[i] = response;                       /* add in a letter */
      i++;
    }
  }
  myString[i] = 0;                          /* terminal NULL character */
}

void USARTClass::printByte(uint8_t byte) {
              /* Converts a byte to a string of decimal text, sends it */
  transmitByte('0' + ( byte / 100));                        /* Hundreds */
  transmitByte('0' + ((byte / 10) % 10));                      /* Tens */
  transmitByte('0' + ( byte % 10));                             /* Ones */
}

void USARTClass::printWord(uint16_t word) {
    transmitByte('0' + ( word / 10000)      );                 /* Ten-thousands */
    transmitByte('0' + ((word /  1000) % 10));               /* Thousands */
    transmitByte('0' + ((word /   100) % 10));                 /* Hundreds */
    transmitByte('0' + ((word /    10) % 10));                      /* Tens */
    transmitByte('0' + ( word          % 10));                             /* Ones */
}


void USARTClass::printBinaryWord(uint16_t word){
	printBinaryByte((uint8_t)(word >> 8));
	printBinaryByte((uint8_t)(word & 0x00FF));
}

void USARTClass::printBinaryByte(uint8_t byte) {
                       /* Prints out a byte as a series of 1's and 0's */
  uint8_t bit;
  for (bit = 7; bit < 255; bit--) {
    if (bit_is_set(byte, bit))
      transmitByte('1');
    else
      transmitByte('0');
  }
}
void USARTClass::printBinaryNibble(uint8_t nibble) {
	/* Prints out a nibble as a series of 1's and 0's */
	uint8_t bit;
	for (bit = 3; bit < 15; bit--) {
		if (bit_is_set(nibble, bit))
		transmitByte('1');
		else
		transmitByte('0');
	}
}

char USARTClass::nibbleToHexCharacter(uint8_t nibble) {
                                   /* Converts 4 bits into hexadecimal */
  if (nibble < 10) {
    return ('0' + nibble);
  }
  else {
    return ('A' + nibble - 10);
  }
}

void USARTClass::printHexByte(uint8_t byte) {
                        /* Prints a byte as its hexadecimal equivalent */
  uint8_t nibble;
  nibble = (byte & 0b11110000) >> 4;
  transmitByte(nibbleToHexCharacter(nibble));
  nibble = byte & 0b00001111;
  transmitByte(nibbleToHexCharacter(nibble));
}

void USARTClass::printHexWord(uint16_t word){
    printHexByte((uint8_t)(word >> 8));
    printHexByte((uint8_t) word);
}


uint8_t USARTClass::getNumber(void) {
  // Gets a numerical 0-255 from the serial port.
  // Converts from string to number.
  char hundreds = '0';
  char tens = '0';
  char ones = '0';
  char thisChar = '0';
  do {                                                   /* shift over */
    hundreds = tens;
    tens = ones;
    ones = thisChar;
    thisChar = receiveByte();                   /* get a new character */
    transmitByte(thisChar);                                    /* echo */
  } while (thisChar != '\n');                     /* until type return */
  return (100 * (hundreds - '0') + 10 * (tens - '0') + ones - '0');
}

uint16_t USARTClass::getNumber16(){
    char man       = '0';
    char thousands = '0';
    char hundreds  = '0';
    char tens      = '0';
    char ones      = '0';
    char thisChar  = '0';

    do{
        man       = thousands;
        thousands = hundreds;
        hundreds  = tens;
        tens      = ones;
        ones      = thisChar;
        thisChar  = receiveByte();
        transmitByte(thisChar);
    } while(thisChar != '\n');
    
//    transmitByte('\n');
    return (10000 * (man - '0') + 1000 * (thousands - '0') + 100 * (hundreds - '0') + 10 * (tens - '0') + ones - '0');
}

/*void USARTClass::printFloat(float number){
    float num = round(number * 100) / 100;
    transmitByte('0' + num / 10);
    transmitByte('0' + num - 10 * floor(num / 10));
    transmitByte('.');
    transmitByte('0' + (num * 10) - floor(num) * 10);
    transmitByte('0' + (num * 100) - floor(num * 10) * 10);
//    printString("\r\n");
}*/

void USARTClass::printFloat(float number){
    float num = round(number * 100) / 100;
    if(num<0){
        num = -num;
        transmitByte('-');
    }

    transmitByte('0' + num / 10);
    transmitByte('0' + num - 10 * floor(num / 10));
    transmitByte('.');
    transmitByte('0' + (num *   10) - floor(num)       * 10);
    transmitByte('0' + (num *  100) - floor(num * 10)  * 10);
//    printString("\r\n");
}

void USARTClass::printNibble(uint8_t nibble){
    transmitByte(nibbleToHexCharacter(nibble));
}

void USARTClass::printROM(uint16_t address, uint16_t n, uint8_t *buff){
    for(uint16_t i=0; i<n; i++){
        if(i>0 && i%8==0){printString("\n");}
        printString("(@ ");
        printWord(address + i);
        printString(":");
        printByte(buff[i]);
        printString(")");
        //printString("\n");
    }
}

void USARTClass::printString_Progmem(const char *strP){
    char oneLetter;
    while(oneLetter = pgm_read_byte(strP)){ // 取り出した値を変数に代入し、その代入値がnullでない限り、～
        transmitByte(oneLetter);
        strP++;
    }
}

USARTClass::USARTClass(){}

// 実装ファイルの中で先に実体化しておく。何もしていないので省略可。
USARTClass usart;
