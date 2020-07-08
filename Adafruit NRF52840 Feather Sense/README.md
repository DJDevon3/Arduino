# Simple Offline Indoor Weather Station Demo
Required hardware: Adafruit NRF52840 Feather Sense board and 3.5" TFT Featherwing.

Also required for the demo are all of the following graphics, tft, and sensor libraries:
* Adafruit_GFX
* Adafruit_HX8357
* Adafruit_APDS9960
* Adafruit_BMP280
* Adafruit_LIS3MDL
* Adafruit_LSM6DS33
* Adafruit_SHT31
* (Arudino library manager will ask during library install to install associated dependency libraries, choose yes).

The demo is a very basic offline real-time sensor demo showing all 10 of the Feather Sense's onboard sensors on the TFT display. The RGB values are part of the APDS9960 light sensor.

 ![](https://raw.githubusercontent.com/DJDevon3/Arduino/master/Adafruit%20NRF52840%20Feather%20Sense/DJDevon3_Simple_Offline_Weatherstation.jpg)
 
# My Custom Offline Indoor Weather Station
Once you play around with the demo you'll see how easy it is to customize it using setCursor positions for text and variables. Customize to your hearts content.

 ![](https://raw.githubusercontent.com/DJDevon3/Arduino/master/Adafruit%20NRF52840%20Feather%20Sense/DJDevon3_MyCustom_Offline_Weatherstation.jpg)
 
My custom sketck uses setCursor positioning, temp sensor buffer, and slightly improved code. There are 5 different temperature ranges which change the temp font color. If all you want is a basic indoor temp monitor then this is likely the one you'll want to run instead of the sensor demo. The code is slightly more advanced than the demo and more customizable if you aren't a beginner.
 
  ![](https://github.com/DJDevon3/Arduino/blob/master/Adafruit%20NRF52840%20Feather%20Sense/DJDevon3_MyCustom_Offline_Weatherstation_Humidity.jpg)
  
  # BLE Timestamp Indoor Weather Station
  My BLE (Bluetooth Low Energy) sketch is the most advanced sketch out of the group. The NRF52840 will show up as "Bluefruit52" on your iOS device. Bluefruit52 is the standard name given to Adafruit's NRF52xxx bluetooth capable boards. This Arduino sketch has BLE timestamp integration. This sketch includes more advanced code than the others including a *newly updated battery voltage & USB voltage display.
  
  * Adafruit recommends you install Adafruits BLE Connect App (free from Apple store) to control your Bluefruit Sense board from your iOS device.
  https://learn.adafruit.com/bluefruit-le-connect
  
  ![](https://github.com/DJDevon3/Arduino/blob/master/Adafruit%20NRF52840%20Feather%20Sense/DJDevon3_BLE_Weatherstation_ios_pairing.jpg)
  
  The Bluefruit Sense board does not have a real-time clock (method to keep accurate time by itself). After paired it will display the current date/time pulled from your iOS device onto the 3.5" TFT. Currrent timestamp is the only data transfered from your iOS device. After disconnecting from bluetooth the weatherstation reverts the first line back to whatever message you've customized it to display. It requires an iOS device connected to show the timestamp in real-time. My BLE sketch will not pair with an Android device to my knowledge and I have no intention on updating it for Android at this time.
  
  ![](https://github.com/DJDevon3/Arduino/blob/master/Adafruit%20NRF52840%20Feather%20Sense/DJDevon3_BLE_Weatherstation.jpg)
  
   # BLE Timestamp Indoor Weather Station with SD Card Wallpaper
   Same sketch as the BLE Timestamp Indoor Weather Station except this one integrates the SD Card function of the 3.5" TFT Featherwing. Sets a wallpaper background image first then fills in weatherstation text over it. I've left some correctly formatted 320x480 24-Bit formatted BMP wallpapers in the repository. Wallpapers are named: astral, grdark, horizon, ocean, purbokeh, sunrise, and texdark. 
   
   There is a problem with characters requiring updates to require a black background to be correctly updated. This issue cannot be seen if you use a solid color black background, however if you add a wallpaper the issue becomes extremely obvious. To my knowledge that issue is within the Adafruit GFX or HX8357 TFT library.
  
  ![](https://github.com/DJDevon3/Arduino/blob/master/Adafruit%20NRF52840%20Feather%20Sense/DJDevon3_BLE_Weatherstation_Character_BGColor.jpg)

  
  
