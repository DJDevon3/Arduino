// Dual LED Strip Music VU Meter by DJDevon3
// https://github.com/DJDevon3/SparkFun_Spectrum
// Based on script by The Red Team (Just Call Me KoKo on YouTube)
// https://github.com/the-red-team/Arduino-FastLED-Music-Visualizer

// Demo designed for Sparkfun Spectrum Shield, audio in, and 2 addressable LED strips (Neopixel, WS2811, WS2812B, etc..)
// Both strips react independently. Cylon tracer jumps from Strip1 to Strip2.
// Both strips react to the lowest frequency 67Hz.

// Pin A0 reserved for Left Channel Audio Input (connected to 3.5mm audio jack and Audio in through-holes)
// Pin A1 reserved for Right channel Audio Input
// Pin D4 reserved for STROBE
// Pin D5 reserved for RESET
// THESE ARE TRACED INTO THE SPARKFUN PCB to the MSGEQ7 chips. They cannot be changed without hardware modification.

#include <FastLED.h>
#define PIN1 2
#define PIN2 3
#define NUM_LEDS_PER_STRIP1 150   // I don't have different strip lengths to test it with. :/
#define NUM_LEDS_PER_STRIP2 150   // For only 2 strips keep it simple, no strip array necessary yet.
#define LED_TYPE            WS2812B 
#define COLOR_TYPE          RGB

// POWER MANAGEMENT (FastLED power management works exceptionally well. It will dim the lights automatically instead of frying your PSU or LED's. Awesome safety feature. Ensure you set the proper values.)
#define POWER_MGMT_VOLTS    5     // In volts. Neopixels, WS2811, WS2812B's are usually 5V.
#define POWER_MGMT_MAMPS    500  // In milliamps. Set a little lower than max amperage of your PSU.

CRGB STRIP1[NUM_LEDS_PER_STRIP1];
CRGB STRIP2[NUM_LEDS_PER_STRIP2];

// GLOBAL CONSTANTS FOR SPECTRUM SHIELD
int strobe = 4;
int reset = 5;
int audio1 = A0;
int audio2 = A1;
int left[7];
int right[7];
int band;
int audio_input = 0;
int freq = 0;

// MULTI-STRIP GLOBALS
int midway1 = NUM_LEDS_PER_STRIP1 / 2;
int midway2 = NUM_LEDS_PER_STRIP2 / 2; 
long VU_METER1 = 0; // Real-time spike for decay
long VU_METER2 = 0;
long react1 = 0; // Real-time adjustment for # of LED's being lit.
long react2 = 0;

// GLOBALS
int loop_max = 0;
int k = 255; // COLOR WHEEL POSITION
int decay = 0; // HOW MANY MS BEFORE ONE LIGHT DECAY. Feel free to customize.
int decay_check = 0;
int wheel_speed = 2;


void setup() {
  delay( 3000 ); // power-up safety delay
  int max_bright = 200; // 0-255  No matter how bright you make it the power management will kick in for safety and dim the lights so you won't fry something.
  FastLED.setBrightness(max_bright);
  set_max_power_in_volts_and_milliamps(POWER_MGMT_VOLTS, POWER_MGMT_MAMPS); // FastLED power management
  
  // MULTIPLEXOR INITIALIZATION
  pinMode(audio1, INPUT);     // Left Channel Pin A0 reserved
  pinMode(audio2, INPUT);     // Right Channel Pin A1 reserved
  pinMode(strobe, OUTPUT);    // MSGEQ7 Multiplexor Strobe Pin 4 reserved
  pinMode(reset, OUTPUT);     // MSGEQ7 Multiplexor Reset Pin 5 reserved 
  digitalWrite(reset, LOW);   // RESET LOW enables STROBE pin
  digitalWrite(strobe, HIGH); // RESET HIGH resets the multiplexor

  // Set Strips
  FastLED.addLeds<LED_TYPE, PIN1, COLOR_TYPE>(STRIP1, NUM_LEDS_PER_STRIP1);
  FastLED.addLeds<LED_TYPE, PIN2, COLOR_TYPE>(STRIP2, NUM_LEDS_PER_STRIP2);
  
  //Serial.begin(115200);
  Serial.begin(9600);
}

void Cylon1() { // Cylon for Strip1
  for(int i = 0; i < NUM_LEDS_PER_STRIP1; i++) {
    STRIP1[i].nscale8(250); 
  } 
}
void Cylon2() { // Cylon for Strip2
  for(int i = 0; i < NUM_LEDS_PER_STRIP2; i++) {
    STRIP2[i].nscale8(250); 
  } 
}

// Color Wheel Generator (rainbow generator)
// https://github.com/NeverPlayLegit/Rainbow-Fader-FastLED/blob/master/rainbow.ino
CRGB Scroll(int pos) {
  CRGB color (0,0,0);
  if(pos < 85) {
    color.g = 0;
    color.r = ((float)pos / 85.0f) * 255.0f;
    color.b = 255 - color.r;
  } else if(pos < 170) {
    color.g = ((float)(pos - 85) / 85.0f) * 255.0f;
    color.r = 255 - color.g;
    color.b = 0;
  } else if(pos < 256) {
    color.b = ((float)(pos - 170) / 85.0f) * 255.0f;
    color.g = 255 - color.b;
    color.r = 1;
  }
  return color;
}

// RAINBOW FADER (Decay Effect): Original went backwards, modified to send waves from first LED. Use this for a rainbow VU meter that begins from the end of a strip.
// https://github.com/NeverPlayLegit/Rainbow-Fader-FastLED/blob/master/rainbow.ino
void singleRainbow(){
  for(int i = NUM_LEDS_PER_STRIP1 - 1; i >= 0; i--) {
    if (i < react1){
      STRIP1[i] = Scroll((i * 256 / 50 + k) % 256);
    }else{
      STRIP1[i] = CRGB(0, 0, 0);
    }  
  }
  for(int i = NUM_LEDS_PER_STRIP2 - 1; i >= 0; i--) {
    if (i < react2){
      STRIP2[i] = Scroll((i * 256 / 50 + k) % 256);
    }else{
      STRIP2[i] = CRGB(0, 0, 0);
    }  
  }
  FastLED.show(); 
}

// Use this instead of singleRainbow for a midpoint mirrored rainbow VU meter
void doubleRainbow(){
  for(int i = NUM_LEDS_PER_STRIP1 - 1; i >= midway1; i--) {
    if (i < react1 + midway1) {
      STRIP1[i] = Scroll((i * 256 / 50 + k) % 256);
      STRIP1[(midway1 - i) + midway1] = Scroll((i * 256 / 50 + k) % 256);
    }
    else
      STRIP1[i] = CRGB(0, 0, 0);
      STRIP1[midway1 - react1] = CRGB(0, 0, 0);
  }
    for(int i = NUM_LEDS_PER_STRIP2 - 1; i >= midway2; i--) {
    if (i < react2 + midway2) {
      STRIP2[i] = Scroll((i * 256 / 50 + k) % 256);
      STRIP2[(midway2 - i) + midway2] = Scroll((i * 256 / 50 + k) % 256);
    }
    else
      STRIP2[i] = CRGB(0, 0, 0);
      STRIP2[midway2 - react2] = CRGB(0, 0, 0);
  }
  FastLED.show();
}

/*
The seven band graphic equalizer IC is a CMOS chip that divides the audio spectrum into seven bands, 63Hz, 160Hz, 400Hz, 1kHz, 2.5kHz, 6.25kHz and 16kHz
After the first strobe leading edge, 63Hz output is on OUT.  Each additional STROBE leading edge advances the multiplexor one channel (63Hz, 160Hz, 400Hz, 1kHz, 2.5kHz, 6.25kHz, 16kHz etc.) and this will repeat indefinitely.
The multiplexor read rate is also the output decay time control.  Each read decays that channel approximately 10%.

To my knowledge this is how the multiplexor works.
RESET HIGH resets the multiplexor
RESET LOW enables STROBE pin
STROBE HIGH selects band
STROBE LOW latches to the band
You cannot shift backwards, only reset or cycle forward.
*/

// Function to read 7 bands.
void readMSGEQ7(){
  digitalWrite(reset, HIGH);
  digitalWrite(reset, LOW);
  for (band = 0; band < 7; band++)
  {
    digitalWrite(strobe, LOW); // strobe pin on the shield - kicks the IC up to the next band 
    delayMicroseconds(30); // 
    left[band] = analogRead(0); // store left band reading
    right[band] = analogRead(1); // ... and the right
    digitalWrite(strobe, HIGH);
  }
}

// For singleRainbow react
void convertSingle(){
  if (left[freq] > right[freq]){
    audio_input = left[freq];
  } else {
    audio_input = right[freq];
  }

  if (audio_input > 80){ // If amplitude greater than 80 set VU level. Example: 150 LEDS X 81 Audio Input Level /1023 = 11 VU level.
    VU_METER1 = ((long)NUM_LEDS_PER_STRIP1 * (long)audio_input) / 1023L; // TRANSLATE AUDIO LEVEL TO NUMBER OF LEDs
    VU_METER2 = ((long)NUM_LEDS_PER_STRIP2 * (long)audio_input) / 1023L; // TRANSLATE AUDIO LEVEL TO NUMBER OF LEDs

    if ((VU_METER1 > react1) && (VU_METER2 > react2)){ // ONLY ADJUST LEVEL OF LED IF LEVEL HIGHER THAN CURRENT LEVEL
      react1 = VU_METER1;
      react2 = VU_METER2;
      Serial.print("Audio Input: ");  // React & VU_METER values should all be identical at this point, waste of resources to print more than one set.
      Serial.print(audio_input);      // Monitor Audio band input
      Serial.print(" Active LEDs: "); // Minimum value in order to be logged is 20. Useful for graphing volume vs strip amperage demand.
      Serial.println(VU_METER1*4);    // How many LEDs should be lit depending on audio signal. Multiplied by 4 for 2 strips. With 2 strips of 150 LEDS it will show a value up to 300. Works for me, you might need to change it.
    }
  }
}

// For doubleRainbow react
void convertDouble(){
  if (left[freq] > right[freq]){
    audio_input = left[freq];
  } else {
    audio_input = right[freq];
  }

  if (audio_input > 80){ // If amplitude greater than 80 set VU level. Example: 150 LEDS X 81 Audio Input Level /1023 = 11 VU level.
    VU_METER1 = ((long)midway1 * (long)audio_input) / 1023L; // TRANSLATE AUDIO LEVEL TO NUMBER OF LEDs
    VU_METER2 = ((long)midway2 * (long)audio_input) / 1023L; // TRANSLATE AUDIO LEVEL TO NUMBER OF LEDs
    
    if ((VU_METER1 > react1)&&(VU_METER1 > react2)){ // ONLY ADJUST LEVEL OF LED IF LEVEL HIGHER THAN CURRENT LEVEL
      react1 = VU_METER1;
      react2 = VU_METER2;
      Serial.print("Audio Input: ");  // React & VU_METER values should all be identical at this point, waste of resources to print more than one set.
      Serial.print(audio_input);      // Monitor Audio input values
      Serial.print(" Active LEDs: "); // Minimum value in order to be logged is 20. Useful for graphing volume vs strip amperage demand.
      Serial.println(VU_METER1*4);    // How many LEDs should be lit depending on audio signal. Multiplied by 4 for 2 strips. With 2 strips of 150 LEDS it will show a value up to 300. Works for me, you might need to change it.
    }
  }
}

// FUNCTION TO VISUALIZE WITH A SINGLE LEVEL
void singleLevel(){
  readMSGEQ7();
  convertSingle();
  singleRainbow();      // APPLY COLOR
  k = k - wheel_speed;  // SPEED OF COLOR WHEEL
  if (k < 0){           // RESET COLOR WHEEL
    k = 255;
  }
  // REMOVE LEDs
  decay_check++;
  if (decay_check > decay){
    decay_check = 0;
    if (react1 > 0){
      react1--;
    }
    if (react2 > 0){
      react2--;
    }
  }
}

// FUNCTION TO VISUALIZE WITH MIRRORED LEVELS
void doubleLevel(){
  readMSGEQ7();
  convertDouble();
  doubleRainbow();
  k = k - wheel_speed; // SPEED OF COLOR WHEEL
  if (k < 0){ // RESET COLOR WHEEL
    k = 255;
  }
  // REMOVE LEDs
  decay_check++;
  if (decay_check > decay){
    decay_check = 0;
    if (react1 > 0){
      react1--;
    }
    if (react2 > 0){
      react2--;
    }
  }
}

// Add the Cylon effect to jump from strip1 to strip2 with rainbow audio react.
void doubleLevelCylon(){
    static uint8_t hue = 0;
    // First slide the led in one direction
    for(int i = 0; i < NUM_LEDS_PER_STRIP1; i++) {  // Set the i'th led to red 
      STRIP1[i] = CHSV(hue++, 255, 255); // Show the leds
      FastLED.show(); // now that we've shown the leds, reset the i'th led to black
      Cylon1();
      doubleLevel();
      STRIP1[i] = CRGB::Black;
    }
    // After reaching the end go back the other direction
    for(int i = (NUM_LEDS_PER_STRIP1)-1; i >= 0; i--) {
      STRIP1[i] = CHSV(hue++, 255, 255);
      FastLED.show();
      Cylon1();
      doubleLevel();
      STRIP1[i] = CRGB::Black;
    }
    // After completing Strip1 journey jump to Strip2. Makes it appear strips are connected.
    for(int i = 0; i < NUM_LEDS_PER_STRIP2; i++) {
      STRIP2[i] = CHSV(hue++, 255, 255);
      FastLED.show();
      Cylon2();
      doubleLevel();
      STRIP2[i] = CRGB::Black;
    }
    // After reaching the end go back the other direction
    for(int i = (NUM_LEDS_PER_STRIP2)-1; i >= 0; i--) {
      STRIP2[i] = CHSV(hue++, 255, 255);
      FastLED.show();
      Cylon2();
      doubleLevel();
      STRIP2[i] = CRGB::Black;
    }
    // Add to forever loop and it's looped infinitely.
    // This makes the two independent strips appear to be connected in an array. They're not.
    // It's just stopping on one strip the same time it starts on the other. An effective technique for a small # of strips. :)
    // If you plan to create a bunch of independent strips eventually you'll need an array. For a couple of strips no problem.
}

void loop() {
  //singleLevel();
  //doubleLevel();
  doubleLevelCylon(); // Designed to be used with 2 LED strips
}
