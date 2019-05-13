# SparkFun_Spectrum
SparkFun Spectrum shield code examples

The sketch shows the potential of the SparkFun Spectrum Shield. It's the only demo I've seen that shows the plotter working for all 7 bands from the shield in real time. Once you see the multiplexor is real you'll have a better idea of how to integrate it into your project.

 ![](https://raw.githubusercontent.com/DJDevon3/Arduino/master/SparkFun_Spectrum_Shield/SparkFun_Spectrum_Frequency_Demo.JPG)
 This is about 60 seconds worth of reading every band while listening to music from Audio In. From this you can extract individual band data from the array for your graphic equalizer or vu projects. Dubstep looks just like it sounds.

### Analyzer Demo
Processes dual 7 band multiplexors and writes them to serial data which can be viewed in serial monitor or serial plotter. Ultimately, it's a good thing to see for yourself both the potential and limitation of the Uno and Spectrum shield combination.

### ATMega Sample Rate
ATmega based boards (UNO, Nano, Mini, Mega) take about 100 microseconds (0.0001 s) to read an analog input. 1 second = 1000000 microseconds. The ATMega chip cannot exceed 20Hz so a 16Hz crystal oscillator is used. 16Hz = 62500 microseconds. That is the maximum possible sampling rate. It takes a few microseconds to read data so if you're looking for precision you'll want to account for that.

### 30 Microsecond Delay for the MSGEQ7
It's required that the MSGEQ7 be given a 30 microsecond delay. It might be tempting to remove the delay in hopes of faster sampling, that would be a mistake. The delay is specified in the MSGEQ7's datasheet. If you do not give it that precise delay, your sketch will fail to properly read the band data.
 
### Arduino Blank Pin AnalogRead Oscillator
On any Arduino Uno, if an analog input pin isn't connected to anything, the value returned by analogRead() will fluctuate based on a number of factors (e.g. the values of the other analog inputs, how close your hand is to the board, etc.). Run a simple analogRead voltage sketch and put your fingers around the analog pin headers to confirm this. The oscillation can be tied directly back to the on-board 16Hz crystal oscillator. You can confirm this by doing the following:
##### 1. Read any open analog pin (example shown)
##### 2. Plug in an unconnected breadboard wire to the analog pin. (peak shown is plugging it in then reading open air).
What is shown is the Arduino Uno's default behavior without the SparkFun shield attached.

 ![](https://raw.githubusercontent.com/DJDevon3/Arduino/master/SparkFun_Spectrum_Shield/AnalogReadOscillator.JPG)
 
It is the Arduino's analog reader waveform from the crystal oscilliator. You don't need to account for the oscillator in your sketches just be aware it is there and don't worry if it shows up in your plotter. Once you use audio in with music or microphone it will go away.

### Spectrum Shield Reset
The shield's reset button is traced to the RST pin header. When the button is pressed it pulls RST to ground which reloads the Arduino's last sketch. The reset button is the same as using the Arduino's reset button. It's not a true reset only a reload of the last sketch. There is no such thing as a factory reset on an Arduino Uno.

### Spectrum Shield Caution:
If you run my SparkFun Spectrum Frequency demo on an Uno it might be too much processing for the ATMega chip to interrupt. It might refuse to load another sketch while it's infinitely reading and writing data per microsecond, once the sketch is running you cannot stop it. If you need to reset your Arduino I recommend trying a fresh blank sketch for a quick overwrite. It's an intensive sketch for an Uno. I've found in many circumstances the only way to reset the Arduino is to remove the Spectrum shield.

If you have a Spectrum shield then chances are you're going to want to play around with the band array data for band extraction. I look forward to seeing what people come up with. Have fun.
