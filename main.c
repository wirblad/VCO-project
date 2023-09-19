#include <stdio.h>
#include <string.h>
#include <util/delay.h>
#include <stdbool.h>
#include <avr/interrupt.h>
#include <time.h>
#include <avr/io.h>
#include <stdlib.h>
#include <math.h>
#include "analogRead.h"
#include "millis.h"
#include "spi3.h"

#define F_CPU 16000000UL
#define BAUD 9600
#define BAUD_TOL 2

//const float refFreq = 25000000.0;
long refFreq = 25000000;  
const int SINE = 0x2000;                    
const int SQUARE = 0x2028;                  
const int TRIANGLE = 0x2002; 

#define LOW 0
#define HIGH 1

const int FSYNC = 10;

#define BIT_SET(a, b) ((a) |= (1ULL << (b)))
#define BIT_CLEAR(a,b) ((a) &= ~(1ULL<<(b)))
#define BIT_FLIP(a,b) ((a) ^= (1ULL<<(b)))
#define BIT_CHECK(a,b) (!!((a) & (1ULL<<(b))))

unsigned long freq = 5;

int main(){
    
    init_serial();
    //printf("START");
    //BIT_CLEAR(DDRC,VERT_PIN);
	  //BIT_CLEAR(DDRC,HORZ_PIN);
    
    //srand(time(NULL));
    // BIT_SET(DDRB,FSYNC);
    // BIT_SET(DDRB,11);
    // BIT_SET(DDRB,12);
    // BIT_SET(DDRB,13);

    //printf("START");
    //DDRB |= 4;
    //SELECT();
 
    //spi_mode(2);
    init_spi_master();
     _delay_ms(100);
     AD9833reset();
     _delay_ms(100);
    WriteRegister(0x2100);
    _delay_ms(100);
    freq = 440;
    AD9833setFrequency(freq, 0x2028);
    _delay_ms(1000);
    freq = 880;
    AD9833setFrequency(freq, 0x2028);
    //millis_init();
    //sei();
    //AD9833_powerup_reset();
    //AD9833_init();
    //AD9833_freq_load(0, 400);
    //int waveType = SQUARE;
    //BIT_SET(DDRB,FSYNC);

    while(1){

    freq = 523;
    AD9833setFrequency(freq, 0x2028);
    _delay_ms(160);
    freq = 587;
    AD9833setFrequency(freq, 0x2028);
     _delay_ms(160);
     freq = 659;
    AD9833setFrequency(freq, 0x2028);
    _delay_ms(1600);
    freq = 698;
    AD9833setFrequency(freq, 0x2028);
    _delay_ms(160);
    freq = 783;
    AD9833setFrequency(freq, 0x2028);
    _delay_ms(1600);
    freq = 880;
    AD9833setFrequency(freq, 0x2028);
    _delay_ms(1600);
    freq = 987;
    AD9833setFrequency(freq, 0x2028);
    _delay_ms(160);
    freq = 1046;
    AD9833setFrequency(freq, 0x2028);
    _delay_ms(1600);


       //AD9833setFrequency(523, waveType); //C
      //_delay_ms(1000);
      // _delay_ms(160);
      // AD9833setFrequency(587, waveType); //D
      // _delay_ms(160);
      // AD9833setFrequency(659, waveType); //E
      // _delay_ms(1600);
      // AD9833setFrequency(698, waveType); //F
      // _delay_ms(160);
      // AD9833setFrequency(783, waveType); //G
      // _delay_ms(1600);
      // AD9833setFrequency(880, waveType); //A
      // _delay_ms(1600);
      // AD9833setFrequency(987, waveType); //B
      // _delay_ms(160);
      // AD9833setFrequency(1046, waveType); //C
      // _delay_ms(1600);

     }
//return 0;
}

void AD9833reset() {
  WriteRegister(0x100);   // Write '1' to AD9833 Control register bit D8.
}

void AD9833setFrequency(unsigned long frequency, int Waveform) {

  double calc = 268435456; //2^28
  double clock = 25000000; 
  double FreqWord = (frequency * calc) / clock;
  

  printf("TEEST %lf", FreqWord);
  uint32_t FreqWord2 = (uint32_t)FreqWord;

  //unsigned long FreqWord = 4724;
 
  uint16_t LSB = (uint16_t)(FreqWord2 & 0x3FFF);                     
  uint16_t MSB = (uint16_t)((FreqWord2 & 0xFFFC000) >> 14);    //Only lower 14 bits are used for data 
  LSB |= 0x4000;
  MSB |= 0x4000;
  WriteRegister(LSB);                  // Write lower 16 bits to AD9833 registers
  WriteRegister(MSB);                  // Write upper 16 bits to AD9833 registers.
  WriteRegister(0xC000);               // Phase register
  WriteRegister(0x2028);               // Exit & Reset to SINE, SQUARE or TRIANGLE
}

void WriteRegister(uint16_t dat) {
  
  printf("DAT: %d", dat);
  //SPI.setDataMode(SPI_MODE2);
  //digitalWrite(FSYNC, LOW);           // Set FSYNC low before writing to AD9833 registers
  spi_mode(2);
  //PORTB &= ~4;
  //BIT_CLEAR(DDRB,FSYNC);
  //SPI_DDR &= ~(1 << CS);
  //delay(10);                            // Give AD9833 time to get ready to receive data.
  SELECT();
  _delay_ms(10);
  //SPI.transfer(highByte(dat));        // Each AD9833 register is 32 bits wide and each 16
  //SPI.transfer(lowByte(dat));         // bits has to be transferred as 2 x 8-bit bytes.
  uint8_t low = dat & 0xff;
  uint8_t high = (dat>>8) & 0xff;
  printf("LOW: %d",low);
  printf("HIGH: %d",high);
  SPI_SendByte(high);
  SPI_SendByte(low);
  DESELECT();
  //SPI_Transfer(dat);
  //SPI_masterTransmitByte(low);
  //SPI_masterTransmitByte(high);
  //SPI_TransferTx16_SingleCS(low,high);
  //SPI_Transfer(highByte(dat));
  //SPI_Transfer(lowByte(dat));
  //SPI_DDR |= (1 << CS);
  //digitalWrite(FSYNC, HIGH);          //Write done. Set FSYNC high
  //BIT_SET(DDRB,FSYNC);
  //_delay_ms(20);
  //PORTB |= 4;
}