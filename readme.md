general note:

Using MCU Arduino Nano to communicate via SPI with a AD9833 wave generator. The AD9833 is capable of putting out sine,triangle and sqaure wave in frequencies ranging
from 0-12.5 MHz. I will write this using only C, so no Arduino IDE. 

I want to create a music instrument, a one-voice synth. Similar to a VCO in a modular system, but with a arpeggiator and random function. Since the somewhat limited capabilities of using just one module when making music I want to implement it as follows:
 
  - User can choose all notes and a big variation of chords, using chords with many extensions. Full extensions equals all notes in a music key.
  - User can switch octaves and waveforms.
  - The selected root note and following chord will be arpeggiated.
  - Make room for unpredicted and seemingly random behavior of the instrument.
  - User can select tempo/speed of the arpeggiator.

Side note: To make any pratical use of such a simple instrument as this in my own musicmaking I need to make it a bit random and unpredictable. Like a random module in a modular system. (this is based on how I write/play/record music.) This is implemented using the following random modes:

  - MODE 0 = Random mode OFF
  - MODE 1 = Each note to be played will be a random note from the currently selected chord
  - MODE 2 = Each note to be played will be a random note from the currently selected chord and will have a probability of 1/4 to be played.
  - MODE 3 = Each note to be played will be a random note from the currently selected chord and will have a probability of 1/3 to be played.
  - MODE 4 = Each note to be played will be a random note from the currently selected chord and will have a probability of 1/3 to be played with an offset to the tempo.

Thanks to: cslammy for using his/her AVR-SPI-C library (MIT license)

https://github.com/cslammy/AVR-SPI-C-library

Thanks to: HAGIWO for inspiration. His build using Nano and AD9833 got me started on this project.

https://www.youtube.com/watch?v=dKddNuAXWYU

Functions:

  - Pot1: Choose a note starting from A. One of twelve notes can be selected.

  - Pot2: Choose a chord. One of 10 chords can be selected. Maj,Maj7,Maj9,Maj11,Maj13,Min,Min7,Min9,Min11,Min13. Maj13 and Min13 is the same as all the notes in the corresponding key.

  - Pot3: Volume.

  - Pot4: Set the tempo of the arpeggiator. How many milliseconds between each note

  - Pot5: Set one off five random modes.







