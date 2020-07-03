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
 /**************  Devons Garage Weatherstation ***********/
 // This sketch requires an Adafruit Bluefruit Sense & Adafruit 3.5" TFT Featherwing.

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

// TFT
Adafruit_HX8357 tft = Adafruit_HX8357(TFT_CS, TFT_DC, TFT_RST);
// Weatherstation
Adafruit_APDS9960 apds9960; // proximity, light, color, gesture
Adafruit_BMP280 bmp280;     // temperature, barometric pressure
Adafruit_LIS3MDL lis3mdl;   // magnetometer
Adafruit_LSM6DS33 lsm6ds33; // accelerometer, gyroscope
Adafruit_SHT31 sht30;       // humidity

// Weatherstation
uint8_t proximity, tempc, tempf;
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
}


void loop(void) {
// Display Rotation: 0 = portrait mode, 1 = landscape mode, 2 = 180 degrees, 3 = 270 degrees.
    tft.setRotation(1);
    proximity = apds9960.readProximity();
  while (!apds9960.colorDataReady()) {
    delay(5);
  }
  apds9960.getColorData(&r, &g, &b, &c);
 
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
  If you power it via battery you might want to delay the refresh for even longer. 
  */
  delay(1000);
  
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

// All possible font colors in the HX8357 library.
// Black, Blue, Red, Green, Cyan, Magenta, Yellow, White
/************************  TFT DISPLAY OUTPUT  *****************************/
unsigned long Display() {
  unsigned long start = micros();
  tft.setCursor(0, 0);
  tft.setTextColor(HX8357_YELLOW, HX8357_BLACK);
  tft.setTextSize(2);
  tft.println("Devons Garage Weatherstation");
  tft.println("----------------------------------------");
  // Show temp in different colors depending on temperature value & keep centered for 2 digit or 3 digit number.
  //tempf = 95; // Debug temp color (120F is maximum)
  if (tempf <= 49){
    tft.setTextColor(HX8357_CYAN, HX8357_BLACK);
    tft.setCursor(0, 40);
    tft.setTextSize(3);
    tft.println("Temp: ");
    tft.setCursor(135, 80);
  } else if (tempf >= 50 && tempf <= 74) {
    tft.setTextColor(HX8357_GREEN, HX8357_BLACK);
    tft.setCursor(0, 40);
    tft.setTextSize(3);
    tft.println("Temp: ");
    tft.setCursor(135, 80);
  } else if (tempf >= 75 && tempf <= 89) {
    tft.setTextColor(HX8357_YELLOW, HX8357_BLACK);
    tft.setCursor(0, 40);
    tft.setTextSize(3);
    tft.println("Temp: ");
    tft.setCursor(135, 80);
  } else if (tempf >= 90 && tempf <= 94) {
    tft.setTextColor(HX8357_MAGENTA, HX8357_BLACK);
    tft.setCursor(0, 40);
    tft.setTextSize(3);
    tft.println("Temp: ");
    tft.setCursor(135, 80);
  } else if (tempf >= 95 && tempf <= 99) {
    tft.setTextColor(HX8357_RED, HX8357_BLACK);
    tft.setCursor(0, 40);
    tft.setTextSize(3);
    tft.println("Temp: ");
    tft.setCursor(135, 80);
  } else if (tempf >= 100) {
    tft.setTextColor(HX8357_RED, HX8357_BLACK);
    tft.setCursor(0, 40);
    tft.setTextSize(3);
    tft.println("Temp: ");
    tft.setCursor(85, 80);
  } else {
    tft.setTextColor(HX8357_WHITE, HX8357_BLACK);
    tft.setCursor(0, 40);
    tft.setTextSize(3);
    tft.println("Temp: ");
    tft.setCursor(130, 80);
  }
  
  tft.setTextSize(16);
  tft.print(tempf);
  tft.setCursor(410, 80);
  tft.setTextSize(4);
  tft.println("F");
  tft.setCursor(0, 240);
  tft.setTextColor(HX8357_GREEN, HX8357_BLACK);
  tft.setTextSize(3);
  tft.print("\nHumidity:  ");
  tft.print(humidity);
  tft.println(" %");
  tft.setTextSize(3);
  tft.print("Barometer: ");
  tft.println(millibar);

  
  return micros() - start;
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

/* FEATHER SENSE SENSOR FUNCTIONS */
void onPDMdata() {
  // query the number of bytes available
  int bytesAvailable = PDM.available();
 
  // read into the sample buffer
  PDM.read(sampleBuffer, bytesAvailable);
 
  // 16-bit, 2 bytes per sample
  samplesRead = bytesAvailable / 2;
}
