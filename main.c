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

const int SINE = 0x2000;                    
const int SQUARE = 0x2028;                  
const int TRIANGLE = 0x2002;

/*
A major/minor triad is the most basic chord and made out of 3 tone. The 7thchord adds one note, the 9th adds another and so on.
A major/minor 13th chord consists of 7 notes, that is the same as the complete major or minor key.
*/
#define MAJOR     3          
#define MAJOR7    4
#define MAJOR9    5
#define MAJOR11   6
#define MAJOR13   7

#define MINOR     3          
#define MINOR7    4
#define MINOR9    5
#define MINOR11   6
#define MINOR13   7

#define MAJORKEY  1
#define MINORKEY  2

#define MAJORINTERVAL 4
#define MINORINTERVAL 3

#define RANDOMOFF     0             //RANDOM MODE OFF
#define RANDOMMODE1   1             //We will choose a random note from the chordarray to be played
#define RANDOMMODE2   2             //MODE 1 + we will have a 1/4 probability that the current note won't be played this will make for more interesting rythmes.
#define RANDOMMODE3   3             //MODE 1 + we will have a 1/3 probability that the current note won't be played
#define RANDOMMODE4   4             //MODE3 + we will make a small offset to the tempo to create odd rythmes

#define BIT_SET(a, b) ((a) |= (1ULL << (b)))
#define BIT_CLEAR(a,b) ((a) &= ~(1ULL<<(b)))
#define BIT_FLIP(a,b) ((a) ^= (1ULL<<(b)))
#define BIT_CHECK(a,b) (!!((a) & (1ULL<<(b))))

#define TEMPO_PIN   0
#define NOTE_PIN    1
#define CHORD_PIN   2
#define RANDOM_PIN  5

#define TEMPO 1000

uint8_t key = MAJORKEY;               //set default key as major
unsigned long chordArray[7];          //Max number of tones in a chord is 7, the same as the notes in a key.
uint8_t offset = 0;                  
uint8_t probabilityTobePlayed = 0;
uint16_t randomMode = 0;

int main(){
    
  init_serial();
  init_spi_master();
  _delay_ms(100);
  AD9833reset();
  _delay_ms(100);
  WriteRegister(0x2100);
  _delay_ms(100);

  millis_init();
  sei();

  srand(time(NULL));

  int WaveType = SQUARE;                //START VALUE IS SQUARE WAVE
  unsigned long freq = 5;               //START VALUE 5Hz

  volatile unsigned long antalMilliSekunderSenasteBytet = 0;
  uint16_t chord = MAJOR;
  uint8_t currentNote = 0;
  
          
  while(1){

      uint16_t tempo = analogRead(TEMPO_PIN);
      if(randomMode == RANDOMMODE4){
        offset = (rand() % 256);
      }

      if( millis_get() - antalMilliSekunderSenasteBytet > (tempo+offset) ){
      
            antalMilliSekunderSenasteBytet = millis_get();
            
            if(randomMode > RANDOMOFF)
              currentNote = (rand() % chord);
            if(randomMode == RANDOMMODE2)
              probabilityTobePlayed = (rand() % 4);
            if(randomMode == RANDOMMODE3 || randomMode == RANDOMMODE4)
              probabilityTobePlayed = (rand() % 3);
            if(probabilityTobePlayed != 2)
              AD9833setFrequency(chordArray[currentNote], WaveType);
            currentNote++;
            if(currentNote >= chord)
              currentNote = 0;
            // TODO SWITCH KNAPP! SINE,TRIANGLE AND SQAURE    
       }

    freq = getNote(); // the user selects the root note of the chord.
    
    chord = chooseChord();
    if(key == MAJORKEY)
      createMajorKey(freq);
    if(key == MINORKEY)
      createMinorKey(freq);

    setRandomState();

    //unsigned long test = 0;
    //AD9833setFrequency(test, WaveType); //NO SUSTAIN ON NOTES !!!!

  }
return 0;
}

void setRandomState(){

  uint16_t random = analogRead(RANDOM_PIN);

  if(random < 814)
    offset = 0;
  if(random < 204){
    probabilityTobePlayed = 0;
    randomMode = RANDOMOFF;
  }
  if(random >=204 && random < 408)  
    randomMode = RANDOMMODE1;
  if(random >=408 && random < 610)
    randomMode = RANDOMMODE2;
  if(random >=610 && random < 814)
    randomMode = RANDOMMODE3;
  if(random >=814 && random <= 1023)
    randomMode = RANDOMMODE4;
}

/*
The user can choose 10 different chord. Major and minor, basic triad, extended chord 7th,9th,11th,13th.
If one choose 13th the chord contains all 7 notes of the coresponding major/minor key.
*/
int chooseChord(){

  uint16_t chord = analogRead(CHORD_PIN);

  if(chord < 510)
    key = MAJORKEY;
  else  
    key = MINORKEY;
  
  if(chord < 102)
    return MAJOR;
  if(chord >= 102 && chord < 204)
    return MAJOR7;
  if(chord >= 204 && chord < 306)
    return MAJOR9;
  if(chord >= 306 && chord < 408)
    return MAJOR11;
  if(chord >= 408 && chord < 510)
    return MAJOR13;
  if(chord >= 510 && chord < 612)
    return MINOR;
  if(chord >= 612 && chord < 714)
    return MINOR7;
  if(chord >= 714 && chord < 816)
    return MINOR9;
  if(chord >= 816 && chord < 918)
    return MINOR11;
  if(chord >= 918 && chord <= 1023)
    return MINOR13;
}

/*
Fill the chord array with all the notes in the major key.
*/
void createMajorKey(unsigned long freq){
  
  chordArray[0] = freq;       //ROOT NOTE, TONIC
  freq = getNote2(freq, MAJORINTERVAL);   
  chordArray[1] = freq;       //SECOND NOTE, ONE MAJOR INTERVALL UP
  freq = getNote2(freq, MINORINTERVAL);  
  chordArray[2] = freq;       //THIRD NOTE, ONE MINOR INTERVALL UP
  freq = getNote2(freq, MAJORINTERVAL);  
  chordArray[3] = freq;       //FOURTH NOTE, ONE MAJOR INTERVALL UP, this is Major7 extension
  freq = getNote2(freq, MINORINTERVAL);  
  chordArray[4] = freq;       //FIFTH NOTE, ONE MINOR INTERVALL UP, this is Major9 extension
  freq = getNote2(freq, MINORINTERVAL);  
  chordArray[5] = freq;       //SIXTH NOTE, ONE MINOR INTERVALL UP, this is Major11 extension
  freq = getNote2(freq, MAJORINTERVAL);  
  chordArray[6] = freq;       //SEVENTH NOTE, ONE MAJOR INTERVALL UP, this is Major13 extension
  
}

void createMinorKey(unsigned long freq){
  
  chordArray[0] = freq;       //ROOT NOTE, TONIC
  freq = getNote2(freq, MINORINTERVAL);   
  chordArray[1] = freq;       //SECOND NOTE, ONE MINOR INTERVALL UP
  freq = getNote2(freq, MAJORINTERVAL);  
  chordArray[2] = freq;       //THIRD NOTE, ONE MAJOR INTERVALL UP
  freq = getNote2(freq, MINORINTERVAL);  
  chordArray[3] = freq;       //FOURTH NOTE, ONE MINOR INTERVALL UP, this is Minor7 extension
  freq = getNote2(freq, MAJORINTERVAL);  
  chordArray[4] = freq;       //FIFTH NOTE, ONE MAJOR INTERVALL UP, this is Minor9 extension
  freq = getNote2(freq, MINORINTERVAL);  
  chordArray[5] = freq;       //SIXTH NOTE, ONE MINOR INTERVALL UP, this is Minor11 extension
  freq = getNote2(freq, MAJORINTERVAL);  
  chordArray[6] = freq;       //SEVENTH NOTE, ONE MAJOR INTERVALL UP, this is Minor13 extension
  
}

/*
Play note from the chordArray, number of notes to be played depends on the selected chord.
NOT USING
*/
// void playChord(int chord, int WaveType){

//   for(int i = 0; i < chord; i++){

//     AD9833setFrequency(chordArray[i], WaveType);
//     _delay_ms(450);
//   }
// }

/*
Take one freq and returns the freq that is a number of semitones above. 
*/
int getNote2(unsigned long freq, int howmanysemitones){

  for(int i = 0; i<howmanysemitones; i++){
          freq = freq * 1.05946;
          
      }
  return freq;
}

/*
Read pot and select one of 12 notes. If A is 440hz, then next A a octave above is 440*2hz = 880hz.
Between 440hz-880hz we have 12 notes. To move up a semitone one needes to multipli current note with 1.05946
The start freq determines the octave.
*/
int getNote(){

  uint16_t note = analogRead(NOTE_PIN);

      //unsigned long START_FREQ = 440;
      unsigned long START_FREQ = 220;
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
      if (note > 938.63 && note <= 1023){
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

/*
This is the code for writing to the AD9833 registers using SPI.
See the AD9833 data sheet for a better understanding of how this works.
*/
void AD9833setFrequency(unsigned long frequency, int Waveform) {

  double calc = 268435456; //2^28
  double clock = 25000000; 
  double FreqWord = (frequency * calc) / clock;
  
  uint32_t FreqWord2 = (uint32_t)FreqWord;
 
  uint16_t LSB = (uint16_t)(FreqWord2 & 0x3FFF);                     
  uint16_t MSB = (uint16_t)((FreqWord2 & 0xFFFC000) >> 14);    //Only lower 14 bits are used for data 
  LSB |= 0x4000;
  MSB |= 0x4000;
  WriteRegister(LSB);                  // Write lower 16 bits to AD9833 registers
  WriteRegister(MSB);                  // Write upper 16 bits to AD9833 registers.
  WriteRegister(0xC000);               // Phase register
  WriteRegister(Waveform);             // Exit & Reset to SINE, SQUARE or TRIANGLE         
}

void WriteRegister(uint16_t dat) {
        
  spi_mode(2);                        
  SELECT();                             // Set FSYNC low before writing to AD9833 registers
  _delay_ms(10);                        // Give AD9833 time to get ready to receive data.      
  uint8_t low = dat & 0xff;
  uint8_t high = (dat>>8) & 0xff;
  SPI_SendByte(high);                   // Each AD9833 register is 32 bits wide and each 16
  SPI_SendByte(low);                    // bits has to be transferred as 2 x 8-bit bytes.
  DESELECT();                           //Write done. Set FSYNC high
}