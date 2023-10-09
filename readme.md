general note:

Using MCU Arduino Nano to communicate via SPI with a AD9833 wave generator. The AD9833 is capable of putting out sine,triangle and sqaure wave in frequencies ranging
from 0-12.5 MHz. 

I want to create a music instrument, a one-voice synth. Similar to a VCO in a modular system, but with a arpeggiator and random function. Since the somewhat limited capabilities of just one module I want to implement it as follows:
 
  - User can choose all notes and a big variation of chords, using chords with many extensions. Full extensions equals all notes in a music key.
  - User can switch octaves and waveforms.
  - The selected root note and following chord will be arpeggiated.
  - Make room for unpredicted and seemingly random behavior of the instrument.
  - User can select tempo/speed of the arpeggiator.

Side note: To make any pratical use of such a simple instrument as this in my own musicmaking I need to make it a bit random and unpredictable. Like a random module in a modular system. (this is my personal belief based on how I write/play/record music.)

23-10-09

Pot1: Choose a note starting from A. One of twelve notes can be selected.
Pot2: Choose a chord. One of 10 chords can be selected. Maj,Maj7,Maj9,Maj11,Maj13,Min,Min7,Min9,Min11,Min13. Maj13 and Min13 is the same as all the notes in the corresponding key.
Pot3: Volume.





