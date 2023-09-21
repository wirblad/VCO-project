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

//#define F_CPU 16000000UL
//#define BAUD 9600
//#define BAUD_TOL 2

//const float refFreq = 25000000.0;
//long refFreq = 25000000;

const int SINE = 0x2000;                    
const int SQUARE = 0x2028;                  
const int TRIANGLE = 0x2002; 

//#define LOW 0
//#define HIGH 1

//const int FSYNC = 10;

#define BIT_SET(a, b) ((a) |= (1ULL << (b)))
#define BIT_CLEAR(a,b) ((a) &= ~(1ULL<<(b)))
#define BIT_FLIP(a,b) ((a) ^= (1ULL<<(b)))
#define BIT_CHECK(a,b) (!!((a) & (1ULL<<(b))))

#define NOTE_PIN 1

int main(){
    
  init_serial();
  //srand(time(NULL));
  init_spi_master();
  _delay_ms(100);
  AD9833reset();
  _delay_ms(100);
  WriteRegister(0x2100);
  _delay_ms(100);

  millis_init();
  sei();

  unsigned long freq = 5;   //START VALUE
  int WaveType = SQUARE;    //START VALUE

  volatile antalMilliSekunderSenasteBytet = 0;                  //TODO: Kontrollera datatyper för att spara plats!!!! KONTROLLERA NOTE SELECT NOGA!!!!!
    
  while(1){

      if( millis_get() - antalMilliSekunderSenasteBytet > 100 ){
      
            // SWITCH KNAPP!
        
       }


    freq = getNote();
    playChord(freq, WaveType);
    //AD9833setFrequency(freq, WaveType);
    antalMilliSekunderSenasteBytet = millis_get();

  }


return 0;
}

void playChord(unsigned long freq, int WaveType){

    unsigned long START_FREQ = freq;
    int howmanysemitones = 4;                 //M7 chord

    for(int i = 0; i<howmanysemitones; i++){
          
          AD9833setFrequency(freq, WaveType);
          _delay_ms(200);
          freq = freq * 1.05946; //next semitone
          freq = freq * 1.05946; //next semitone
          freq = freq * 1.05946; //next semitone
      }
}

/*
Read pot and select one of 12 notes. If A is 440hz, then next A a octabe above is 440*2hz = 880hz.
Between 440hz-880hz we have 12 notes.
*/
int getNote(){

  uint16_t note = analogRead(NOTE_PIN);

      unsigned long START_FREQ = 440;
      unsigned long freq = START_FREQ;
      int howmanysemitones = 0;

      if (note <= 85.33){
        howmanysemitones=0;
      }
      if (note > 85.33 && note <= 170.66){
        howmanysemitones=1;
      }
      if (note > 170.66 && note <= 256){
        howmanysemitones=2;
      }
      if (note > 256 && note <= 341.32){
        howmanysemitones=3;
      }
      if (note > 341.32 && note <= 426.65){
        howmanysemitones=4;
      }
      if (note > 426.65 && note <= 511.98){
        howmanysemitones=5;
      }
      if (note > 511.98 && note <= 597.31){
        howmanysemitones=6;
      }
      if (note > 597.31 && note <= 682.64){
        howmanysemitones=7;
      }
      if (note > 682.64 && note <= 767.97){
        howmanysemitones=8;
      }
      if (note > 767.97 && note <= 853.3){
        howmanysemitones=9;
      }
      if (note > 853.3 && note <= 938.63){
        howmanysemitones=10;
      }
      if (note > 938.63 && note <= 1024){
        howmanysemitones=11;
      }

      for(int i = 0; i<howmanysemitones; i++){
          freq = START_FREQ * 1.05946;
          START_FREQ = freq;
      }
      return freq;
}

void AD9833reset() {
  WriteRegister(0x100);   // Write '1' to AD9833 Control register bit D8.
}

void AD9833setFrequency(unsigned long frequency, int Waveform) {

  double calc = 268435456; //2^28
  double clock = 25000000; //internal osc clock AD9833
  double FreqWord = (frequency * calc) / clock;
  
  printf("TEEST %lf", FreqWord);
  uint32_t FreqWord2 = (uint32_t)FreqWord;
 
  uint16_t LSB = (uint16_t)(FreqWord2 & 0x3FFF);                     
  uint16_t MSB = (uint16_t)((FreqWord2 & 0xFFFC000) >> 14);    //Only lower 14 bits are used for data 
  LSB |= 0x4000;
  MSB |= 0x4000;
  WriteRegister(LSB);                  // Write lower 16 bits to AD9833 registers
  WriteRegister(MSB);                  // Write upper 16 bits to AD9833 registers.
  WriteRegister(0xC000);               // Phase register
  WriteRegister(Waveform);             // Exit & Reset to SINE, SQUARE or TRIANGLE
  //WriteRegister(0x2028);               
}

void WriteRegister(uint16_t dat) {
  
  printf("DAT: %d", dat);
           
  spi_mode(2);
  //delay(10);                          // Give AD9833 time to get ready to receive data.
  SELECT();                             // Set FSYNC low before writing to AD9833 registers
  _delay_ms(10);       
  uint8_t low = dat & 0xff;
  uint8_t high = (dat>>8) & 0xff;
  printf("LOW: %d",low);
  printf("HIGH: %d",high);
  SPI_SendByte(high);                   // Each AD9833 register is 32 bits wide and each 16
  SPI_SendByte(low);                    // bits has to be transferred as 2 x 8-bit bytes.
  DESELECT();                           //Write done. Set FSYNC high
}