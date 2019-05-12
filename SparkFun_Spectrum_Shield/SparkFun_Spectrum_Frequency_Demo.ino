/* SPARKFUN FREQUENCY SPECTRUM DEMO
// Pin A0 of the shield is reserved for Left Audio
// Pin A1 of the shield is reserved for Right Audio
// Pin D4 of the shield is reserved for STROBE
// Pin D5 of the shield is reserved for RESET.  They are hardwired as traces in the PCB to the MSGEQ7 chips. These can't be changed without hardware modification.

// Plug music into the Audio In jack, run this sketch, and turn on the plotter (CTRL+SHIFT+L in Arudino IDE).
// Ensure you set the plotter baud rate to read the same rate you set in void setup.
// If your shield works correctly you should see 7 frequency bands plotted to the music.
*/
int strobe = 4;
int reset = 5;
int left[7];
int right[7];
int band;

void setup() {
  // 115200 baud is 12X faster than 9600 however it will produce random glitchy errors. They're very obvious in the serial monitor.
  // 9600 is better for coding & troubleshooting to avoid errors. Keep that in mind if you start coding a big script.
  //Serial.begin(115200);
   Serial.begin(9600);
}

/*
The seven band graphic equalizer IC is a CMOS chip that divides the audio spectrum into seven bands, 63Hz, 160Hz, 400Hz, 1kHz, 2.5kHz, 6.25kHz and 16kHz
After the first strobe leading edge, 63Hz output is on OUT.  Each additional STROBE leading edge advances the multiplexor one channel (63Hz, 160Hz, 400Hz, 1kHz, 2.5kHz, 6.25kHz, 16kHz etc.) and this will repeat indefinitely.
The multiplexor read rate is also the output decay time control.  Each read decays that channel approximately 10%.
*/

void readMSGEQ7(){
  // Reads all 14 bands simultaneously (7 left channel + 7 right channel)
  digitalWrite(reset, HIGH);
  digitalWrite(reset, LOW);
  for (band = 0; band < 7; band++){
    digitalWrite(strobe, LOW);
    delayMicroseconds(30);
    left[band] = analogRead(0); 
    right[band] = analogRead(1); 
    digitalWrite(strobe, HIGH);
  }
}

void displayMSGEQ7(){ // Displays 7 bands in serial monitor and plotter
  for (band = 0; band < 7; band++){ 
    Serial.print(left[band]);
    Serial.print(" ");
  }
  Serial.println(" - Left ");
  for (band = 0; band < 7; band++){ 
    Serial.print(right[band]);
    Serial.print(" ");
  }
  Serial.println(" - Right" );
}

void loop() {
  readMSGEQ7();
  displayMSGEQ7();
}
