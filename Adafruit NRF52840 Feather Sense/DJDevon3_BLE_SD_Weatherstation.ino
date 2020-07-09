/***************************************************
  This is our library for the Adafruit 3.5" TFT (HX8357) FeatherWing
  ----> http://www.adafruit.com/products/3651

  Check out the links above for our tutorials and wiring diagrams
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional)
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/
 /* Simple Offline Indoor Weather Station using Adafruit NRF52840 Bluefruit Feather Sense & Adafruit 3.5" TFT by DJDevon3 */
 /**************  Devons Garage Weatherstation with BLE Date/Time ***********/
 // This sketch requires an Adafruit Feather Bluefruit Sense Board & Adafruit 3.5" TFT Featherwing (with a Micro SD card plugged into the Featherwing TFT).

#include <SPI.h>
#include "Adafruit_GFX.h"
#include "Adafruit_HX8357.h"
#include <Adafruit_APDS9960.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_LIS3MDL.h>
#include <Adafruit_LSM6DS33.h>
#include <Adafruit_SHT31.h>
#include <Adafruit_Sensor.h>
#include <PDM.h>
#include <SD.h>
#include <bluefruit.h>

#ifdef ESP8266
   #define STMPE_CS 16
   #define TFT_CS   0
   #define TFT_DC   15
   #define SD_CS    2
#endif
#ifdef ESP32
   #define STMPE_CS 32
   #define TFT_CS   15
   #define TFT_DC   33
   #define SD_CS    14
#endif
#ifdef TEENSYDUINO
   #define TFT_DC   10
   #define TFT_CS   4
   #define STMPE_CS 3
   #define SD_CS    8
#endif
#ifdef ARDUINO_STM32_FEATHER
   #define TFT_DC   PB4
   #define TFT_CS   PA15
   #define STMPE_CS PC7
   #define SD_CS    PC5
#endif
#ifdef ARDUINO_NRF52832_FEATHER
   #define STMPE_CS 30
   #define TFT_CS   31
   #define TFT_DC   11
   #define SD_CS    27
#endif
#if defined(ARDUINO_MAX32620FTHR) || defined(ARDUINO_MAX32630FTHR)
   #define TFT_DC   P5_4
   #define TFT_CS   P5_3
   #define STMPE_CS P3_3
   #define SD_CS    P3_2
#endif

// Anything else!
#if defined (__AVR_ATmega32U4__) || defined(ARDUINO_SAMD_FEATHER_M0) || defined (__AVR_ATmega328P__) || \
defined(ARDUINO_SAMD_ZERO) || defined(__SAMD51__) || defined(__SAM3X8E__) || defined(ARDUINO_NRF52840_FEATHER) || defined(ARDUINO_NRF52840_FEATHER_SENSE)
   #define STMPE_CS 6
   #define TFT_CS   9
   #define TFT_DC   10
   #define SD_CS    5
#endif

#define TFT_RST -1

// BLE Client Current Time Service
BLEClientCts  bleCTime;

// TFT
Adafruit_HX8357 tft = Adafruit_HX8357(TFT_CS, TFT_DC, TFT_RST);
// Feather Sense Sensors
Adafruit_APDS9960 apds9960; // proximity, light, color, gesture
Adafruit_BMP280 bmp280;     // temperature, barometric pressure
Adafruit_LIS3MDL lis3mdl;   // magnetometer
Adafruit_LSM6DS33 lsm6ds33; // accelerometer, gyroscope
Adafruit_SHT31 sht30;       // humidity

// Feather Sense Sensor Variables
uint8_t proximity, tempc, tempcbuffer, tempf, tempfbuffer;
uint16_t r, g, b, c;
float pressure, millibar, altitude, elevation;
float magnetic_x, magnetic_y, magnetic_z;
float accel_x, accel_y, accel_z;
float gyro_x, gyro_y, gyro_z;
float humidity;
int32_t mic;
extern PDMClass PDM;
short sampleBuffer[256];  // buffer to read samples into, each sample is 16-bits
volatile int samplesRead; // number of samples read

void setup() {
  Serial.begin(115200); 
  // Initialize the sensors
  apds9960.begin();
  apds9960.enableProximity(true);
  apds9960.enableColor(true);
  bmp280.begin();
  lis3mdl.begin_I2C();
  lsm6ds33.begin_I2C();
  sht30.begin();
  PDM.onReceive(onPDMdata);
  PDM.begin(1, 16000);

  // Initialize TFT
  tft.begin();
  tft.fillScreen(HX8357_BLACK);
  
  // Initialize SD Card Reader
  Serial.print("Initializing SD card...");
  if (!SD.begin(SD_CS)) { Serial.println("failed!");}
  Serial.println("OK!");
  bmpDraw("/purbokeh.bmp", 0, 0); // Bitmaps must be formated with 24-bit color. MS Paint is an easy way to reformat jpgs to bmps.
  
  // read diagnostics (optional but can help debug problems)
  /*
   * uint8_t x = tft.readcommand8(HX8357_RDPOWMODE);
  Serial.print("Display Power Mode: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(HX8357_RDMADCTL);
  Serial.print("MADCTL Mode: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(HX8357_RDCOLMOD);
  Serial.print("Pixel Format: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(HX8357_RDDIM);
  Serial.print("Image Format: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(HX8357_RDDSDR);
  Serial.print("Self Diagnostic: 0x"); Serial.println(x, HEX); 
  Serial.println(F("Benchmark                Time (microseconds)"));
  */

  // Initialize Bluetooth
  Serial.println("Bluefruit52");
  Serial.println("-------------------------------------------\n");
  Serial.println("Go to iOS's Bluetooth settings and connect to Bluefruit52\n");

  // Config the peripheral connection with maximum bandwidth, more SRAM required by SoftDevice
  // Note: All config***() function must be called before begin()
  Bluefruit.configPrphBandwidth(BANDWIDTH_MAX);
  Bluefruit.begin();
  Bluefruit.setTxPower(4);    // Check bluefruit.h for supported values
  Bluefruit.setName("Bluefruit52");
  Bluefruit.Periph.setConnectCallback(connect_callback);
  Bluefruit.Periph.setDisconnectCallback(disconnect_callback);
  bleCTime.begin(); // Configure CTS client

  // Callback invoked when iOS device time changes
  // To test this go to Setting -> Date & Time -> Toggle Time Zone "Set Automatically"
  // Or change the time manually etc ...
  bleCTime.setAdjustCallback(cts_adjust_callback);

  // Set up and start advertising Bluefruit52
  startAdv();
}


void loop(void) {
// Display Rotation: 0 = portrait mode, 1 = landscape mode, 2 = 180 degrees, 3 = 270 degrees.
    tft.setRotation(1);
    proximity = apds9960.readProximity();
  while (!apds9960.colorDataReady()) {
    delay(5);
  }
  apds9960.getColorData(&r, &g, &b, &c);
 
  tempcbuffer = bmp280.readTemperature();
  tempfbuffer = (tempc*1.8)+32;
  delay(3000);
  tempc = bmp280.readTemperature();
  tempf = (tempc*1.8)+32;
  pressure = bmp280.readPressure();
  millibar = (pressure / 100);
  altitude = bmp280.readAltitude(millibar); // This can be customized by manually entering your local barometric pressure in millibars. Format is (1014.00)
  elevation = (altitude * 3.28); // Converts altitude from meters to feet.
 
  lis3mdl.read();
  magnetic_x = lis3mdl.x;
  magnetic_y = lis3mdl.y;
  magnetic_z = lis3mdl.z;
 
  sensors_event_t accel;
  sensors_event_t gyro;
  sensors_event_t temp;
  lsm6ds33.getEvent(&accel, &gyro, &temp);
  accel_x = accel.acceleration.x;
  accel_y = accel.acceleration.y;
  accel_z = accel.acceleration.z;
  gyro_x = gyro.gyro.x;
  gyro_y = gyro.gyro.y;
  gyro_z = gyro.gyro.z;
  humidity = sht30.readHumidity();
  samplesRead = 0;
  mic = getPDMwave(4000);

  Display(); // Main display loop
  /* Display Refresh Delay
  For a simple indoor weather station a 1 second refresh is plenty. 
  If you power it via battery you might want to delay the refresh for longer or add a TFT sleep state. 
  */
  delay(1000);

  // Skip if bluetooth service not yet discovered
  if ( !bleCTime.discovered() ) return;
  // Skip if service connection is not paired/secured
  if ( !Bluefruit.connPaired( bleCTime.connHandle() ) ) return;
  // Get Time from iOS once per second
  // Note it is not advised to update this quickly
  // Application should use local clock and update time after 
  // a long period (e.g an hour or day)
  bleCTime.getCurrentTime();
  int utc_offset =  bleCTime.LocalInfo.timezone*15; // in 15 minutes unit
  
 // Uncomment below for debugging to Serial Console.
 /*
  Serial.println("\nFeather Sense Sensor Demo");
  Serial.println("----------------------------");
  Serial.print("Proximity: ");
  Serial.println(apds9960.readProximity());
  Serial.print("Red: ");
  Serial.print(r);
  Serial.print(" Green: ");
  Serial.print(g);
  Serial.print(" Blue :");
  Serial.print(b);
  Serial.print(" Clear: ");
  Serial.println(c);
  Serial.print("Temperature: ");
  Serial.print(tempc);
  Serial.print(" C / ");
  Serial.print(tempf);
  Serial.println(" F");
  Serial.print("Barometric pressure: ");
  Serial.println(pressure);
  Serial.print("Altitude: ");
  Serial.print(altitude);
  Serial.println(" m");
  Serial.print("Magnetic: ");
  Serial.print(magnetic_x);
  Serial.print(" ");
  Serial.print(magnetic_y);
  Serial.print(" ");
  Serial.print(magnetic_z);
  Serial.println(" uTesla");
  Serial.print("Acceleration: ");
  Serial.print(accel_x);
  Serial.print(" ");
  Serial.print(accel_y);
  Serial.print(" ");
  Serial.print(accel_z);
  Serial.println(" m/s^2");
  Serial.print("Gyro: ");
  Serial.print(gyro_x);
  Serial.print(" ");
  Serial.print(gyro_y);
  Serial.print(" ");
  Serial.print(gyro_z);
  Serial.println(" dps");
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");
  Serial.print("Mic: ");
  Serial.println(mic);
  delay(3000);
  */
}


// All possible GFX library colors. You must use the hex value not the color name for GFX drawing.
/*
BLACK   0x0000
GRAY    0x8410
WHITE   0xFFFF
RED     0xF800
ORANGE  0xFA60
YELLOW  0xFFE0 
LIME    0x07FF
GREEN   0x07E0
CYAN    0x07FF
AQUA    0x04FF
BLUE    0x001F
MAGENTA 0xF81F
PINK    0xF8FF
*/


// All possible TFT font colors in the HX8357 library.
// HX8357_ prefix then Black, Blue, Red, Green, Cyan, Magenta, Yellow, White
// Second color is background color, good for character refreshes rather than requiring a screen redraw.
/************************  TFT DISPLAY OUTPUT  *****************************/
unsigned long Display() {
  const char * day_of_week_str[] = { "n/a", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday" };
  tft.setCursor(0, 0);
  tft.setTextColor(HX8357_YELLOW, HX8357_BLACK);
  tft.setTextSize(2);
  
  // If connected to Bluetooth device pull & display timestamp
    if (bleCTime.getCurrentTime() !=NULL ){
      tft.print("Date: ");
      tft.print(day_of_week_str[bleCTime.Time.weekday]);
      tft.print(" ");
      tft.print(bleCTime.Time.year);
      tft.print("/");
      tft.print(bleCTime.Time.month);
      tft.print("/");
      tft.print(bleCTime.Time.day);
      tft.print("   Time: ");
      tft.print(bleCTime.Time.hour);
      tft.print(":");
      tft.println(bleCTime.Time.minute);
    } else {
      tft.println("Simple Offline Weatherstation           ");
    }
  tft.println("----------------------------------------");


  // Show temp in different colors depending on temperature value & keep centered for 2 digit or 3 digit number.
  //tempf = 95; // Debug temp color (120F is maximum)
  if (tempf <= 49){
    tft.setTextColor(HX8357_CYAN, HX8357_BLACK);
    tempText();
  } else if (tempf >= 50 && tempf <= 74) {
    tft.setTextColor(HX8357_GREEN, HX8357_BLACK);
    tempText();
  } else if (tempf >= 75 && tempf <= 89) {
    tft.setTextColor(HX8357_YELLOW, HX8357_BLACK);
    tempText();
  } else if (tempf >= 90 && tempf <= 94) {
    tft.setTextColor(HX8357_MAGENTA, HX8357_BLACK);
    tempText();
  } else if (tempf >= 95 && tempf <= 99) {
      if (tempfbuffer >= 100 && tempf <=99){
        tft.fillScreen(HX8357_BLACK);
      }
    tft.setTextColor(HX8357_RED, HX8357_BLACK);
    tempText();
  } else if (tempf >= 100 ) { // If temp uses 3 characters, shift cursor position to stay centered on screen. If you go below 0F in winter, turn on your heater!
    tft.setTextColor(HX8357_RED, HX8357_BLACK);
    tft.setCursor(0, 40);
    tft.setTextSize(3);
    tft.println("Temp");
    tft.setCursor(85, 80);
  } else {
    tft.setTextColor(HX8357_WHITE, HX8357_BLACK);
    tempText();
  }

  tft.setTextSize(16);
  tft.print(tempf);
  tft.setCursor(410, 80);
  tft.setTextSize(4);
  tft.println("F");
  
  tft.setCursor(0, 240);
  tft.setTextColor(HX8357_GREEN);
  tft.setTextSize(3);
  tft.print("Humidity:  ");
  tft.setTextColor(HX8357_GREEN, HX8357_BLACK);
  tft.println(humidity *1.35);
  tft.setCursor(290, 240);
  tft.setTextColor(HX8357_GREEN);
  tft.println(" %");
  tft.setTextSize(3);
  tft.print("Barometer: ");
  tft.setTextColor(HX8357_GREEN, HX8357_BLACK);
  tft.println(millibar); //During hurricanes NOAA reports barometric pressure in millibar.

  // Function for detecting if using USB Power or Battery.
  float batVoltage = checkBattery();  //get battery voltage
  if (batVoltage < 3.95) {
    int batread = analogRead(A6);
    tft.setCursor(0, 290);
    tft.setTextColor(HX8357_GREEN);
    tft.setTextSize(3);
    tft.print("Bat: " ); 
    tft.setTextColor(HX8357_GREEN, HX8357_BLACK);
    tft.print(batVoltage);  //display battery voltage (3.7v battery might slightly overcharge to 3.8v, same for a 4.2v battery)
    tft.setTextColor(HX8357_GREEN);
    tft.print("V");
    // For debugging Battery value. Value approximately 1200 for 3.7v battery and 1690 for 4.2v battery. Useful for calculating battery life.
    // tft.print(" Value:" ); 
    // tft.println(batread * 2);  
    
  } else {
    int batread = analogRead(A6);
    tft.setCursor(0, 290);
    tft.setTextColor(HX8357_GREEN);
    tft.setTextSize(3);
    tft.print("USB: " ); 
    tft.setTextColor(HX8357_GREEN, HX8357_BLACK);
    tft.print(batVoltage);  //display USB voltage (normal USB voltage for Feather Sense is approximately 3.95-3.96 with this calculation)
    tft.setTextColor(HX8357_GREEN);
    tft.print("V");
    // For debugging USB value. Display USB value. Value approximately 1225-1230 for USB. Useful for knowing when the board is USB vs Battery powered.
    // As long as you know the constant value to expect from USB then anything less must be running on a battery... right?
    // tft.print(" Value:" ); 
    // tft.println(batread * 2); // Note: The same batread value is used for both Bat & USB because both power traces on the PCB use pin A6 on the Feather Sense.
    // I'm using a 3.7v PKCell battery (from Adafruit).
  }
 
  delay(100);
}

void tempText(){ // Set a function instead of repeating every line in temp color difference.
    tft.setCursor(0, 40);
    tft.setTextSize(3);
    tft.println("Temp");
    tft.setCursor(135, 80);
}
  // Returns current voltage of 3.7v Lipo battery on a Feather Sense (3.2v-3.8v working range). Voltage should always show 4.2v with no battery plugged in.
  // Calculate battery life percentage from 3.2v to 4.2v
float checkBattery(){
  float measuredvbat = analogRead(A6);
  measuredvbat *= 2;    // we divided by 2, so multiply back
  measuredvbat *= 3.3;  // Multiply by 3.3V, our reference voltage
  measuredvbat /= 1024; // convert to voltage
  return measuredvbat;
}

/* FEATHER SENSE SENSOR FUNCTIONS */
int32_t getPDMwave(int32_t samples) {
  short minwave = 30000;
  short maxwave = -30000;
 
  while (samples > 0) {
    if (!samplesRead) {
      yield();
      continue;
    }
    for (int i = 0; i < samplesRead; i++) {
      minwave = min(sampleBuffer[i], minwave);
      maxwave = max(sampleBuffer[i], maxwave);
      samples--;
    }
    // clear the read count
    samplesRead = 0;
  }
  return maxwave - minwave;
}
void onPDMdata() {
  // query the number of bytes available
  int bytesAvailable = PDM.available();
 
  // read into the sample buffer
  PDM.read(sampleBuffer, bytesAvailable);
 
  // 16-bit, 2 bytes per sample
  samplesRead = bytesAvailable / 2;
}

/************************** Everything below are Bluefruit functions *****************************/
void startAdv(void)
{
  // Advertising packet
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();
  Bluefruit.Advertising.addAppearance(BLE_APPEARANCE_GENERIC_CLOCK);

  // Include CTS client UUID
  Bluefruit.Advertising.addService(bleCTime);

  // Includes name
  Bluefruit.Advertising.addName();
  
  /* Start Advertising
   * - Enable auto advertising if disconnected
   * - Interval:  fast mode = 20 ms, slow mode = 152.5 ms
   * - Timeout for fast mode is 30 seconds
   * - Start(timeout) with timeout = 0 will advertise forever (until connected)
   * 
   * For recommended advertising interval
   * https://developer.apple.com/library/content/qa/qa1931/_index.html   
   */
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244);    // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);      // number of seconds in fast mode
  Bluefruit.Advertising.start(0);                // 0 = Don't stop advertising after n seconds
}

void connect_callback(uint16_t conn_handle)
{
  Serial.println("Connected");
  
  Serial.print("Discovering CTS ... ");
  if ( bleCTime.discover(conn_handle) )
  {
    Serial.println("Discovered");
    
    // iOS requires pairing to work, it makes sense to request security here as well
    Serial.print("Attempting to PAIR with the iOS device, please press PAIR on your phone ... ");
    if ( Bluefruit.requestPairing(conn_handle) )
    {
      Serial.println("Done");
      Serial.println("Enabling Time Adjust Notify");
      bleCTime.enableAdjust();

      Serial.print("Get Current Time chars value");
      bleCTime.getCurrentTime();

      Serial.print("Get Local Time Info chars value");
      bleCTime.getLocalTimeInfo();

      Serial.println();
    }

    Serial.println();
  }
}

void cts_adjust_callback(uint8_t reason)
{
  const char * reason_str[] = { "Manual", "External Reference", "Change of Time Zone", "Change of DST" };

  Serial.println("iOS Device time changed due to ");
  Serial.println( reason_str[reason] );
}

/**
 * Callback invoked when a connection is dropped
 * @param conn_handle connection where this event happens
 * @param reason is a BLE_HCI_STATUS_CODE which can be found in ble_hci.h
 */
void disconnect_callback(uint16_t conn_handle, uint8_t reason)
{
  (void) reason;

  Serial.println();
  Serial.print("Disconnected, reason = 0x"); Serial.println(reason, HEX);
}

// This function opens a Windows Bitmap (BMP) file and
// displays it at the given coordinates.  It's sped up
// by reading many pixels worth of data at a time
// (rather than pixel by pixel).  Increasing the buffer
// size takes more of the Arduino's precious RAM but
// makes loading a little faster.  20 pixels seems a
// good balance.

#define BUFFPIXEL 20

void bmpDraw(char *filename, uint8_t x, uint16_t y) {

  File     bmpFile;
  int      bmpWidth, bmpHeight;   // W+H in pixels
  uint8_t  bmpDepth;              // Bit depth (currently must be 24)
  uint32_t bmpImageoffset;        // Start of image data in file
  uint32_t rowSize;               // Not always = bmpWidth; may have padding
  uint8_t  sdbuffer[3*BUFFPIXEL]; // pixel buffer (R+G+B per pixel)
  uint8_t  buffidx = sizeof(sdbuffer); // Current position in sdbuffer
  boolean  goodBmp = false;       // Set to true on valid header parse
  boolean  flip    = true;        // BMP is stored bottom-to-top
  int      w, h, row, col;
  uint8_t  r, g, b;
  uint32_t pos = 0, startTime = millis();

  if((x >= tft.width()) || (y >= tft.height())) return;

  Serial.println();
  Serial.print(F("Loading image '"));
  Serial.print(filename);
  Serial.println('\'');

  // Open requested file on SD card
  if ((bmpFile = SD.open(filename)) == NULL) {
    Serial.print(F("File not found"));
    return;
  }

  // Parse BMP header
  if(read16(bmpFile) == 0x4D42) { // BMP signature
    Serial.print(F("File size: ")); Serial.println(read32(bmpFile));
    (void)read32(bmpFile); // Read & ignore creator bytes
    bmpImageoffset = read32(bmpFile); // Start of image data
    Serial.print(F("Image Offset: ")); Serial.println(bmpImageoffset, DEC);
    // Read DIB header
    Serial.print(F("Header size: ")); Serial.println(read32(bmpFile));
    bmpWidth  = read32(bmpFile);
    bmpHeight = read32(bmpFile);
    if(read16(bmpFile) == 1) { // # planes -- must be '1'
      bmpDepth = read16(bmpFile); // bits per pixel
      Serial.print(F("Bit Depth: ")); Serial.println(bmpDepth);
      if((bmpDepth == 24) && (read32(bmpFile) == 0)) { // 0 = uncompressed

        goodBmp = true; // Supported BMP format -- proceed!
        Serial.print(F("Image size: "));
        Serial.print(bmpWidth);
        Serial.print('x');
        Serial.println(bmpHeight);

        // BMP rows are padded (if needed) to 4-byte boundary
        rowSize = (bmpWidth * 3 + 3) & ~3;

        // If bmpHeight is negative, image is in top-down order.
        // This is not canon but has been observed in the wild.
        if(bmpHeight < 0) {
          bmpHeight = -bmpHeight;
          flip      = false;
        }

        // Crop area to be loaded
        w = bmpWidth;
        h = bmpHeight;
        if((x+w-1) >= tft.width())  w = tft.width()  - x;
        if((y+h-1) >= tft.height()) h = tft.height() - y;

        // Set TFT address window to clipped image bounds
        tft.startWrite(); // Start TFT transaction
        tft.setAddrWindow(x, y, w, h);

        for (row=0; row<h; row++) { // For each scanline...

          // Seek to start of scan line.  It might seem labor-
          // intensive to be doing this on every line, but this
          // method covers a lot of gritty details like cropping
          // and scanline padding.  Also, the seek only takes
          // place if the file position actually needs to change
          // (avoids a lot of cluster math in SD library).
          if(flip) // Bitmap is stored bottom-to-top order (normal BMP)
            pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
          else     // Bitmap is stored top-to-bottom
            pos = bmpImageoffset + row * rowSize;
          if(bmpFile.position() != pos) { // Need seek?
            tft.endWrite(); // End TFT transaction
            bmpFile.seek(pos);
            buffidx = sizeof(sdbuffer); // Force buffer reload
            tft.startWrite(); // Start new TFT transaction
          }

          for (col=0; col<w; col++) { // For each pixel...
            // Time to read more pixel data?
            if (buffidx >= sizeof(sdbuffer)) { // Indeed
              tft.endWrite(); // End TFT transaction
              bmpFile.read(sdbuffer, sizeof(sdbuffer));
              buffidx = 0; // Set index to beginning
              tft.startWrite(); // Start new TFT transaction
            }

            // Convert pixel from BMP to TFT format, push to display
            b = sdbuffer[buffidx++];
            g = sdbuffer[buffidx++];
            r = sdbuffer[buffidx++];
            tft.pushColor(tft.color565(r,g,b));
          } // end pixel
        } // end scanline
        tft.endWrite(); // End last TFT transaction
        Serial.print(F("Loaded in "));
        Serial.print(millis() - startTime);
        Serial.println(" ms");
      } // end goodBmp
    }
  }

  bmpFile.close();
  if(!goodBmp) Serial.println(F("BMP format not recognized."));
}

// These read 16- and 32-bit types from the SD card file.
// BMP data is stored little-endian, Arduino is little-endian too.
// May need to reverse subscript order if porting elsewhere.

uint16_t read16(File &f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}

uint32_t read32(File &f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}
