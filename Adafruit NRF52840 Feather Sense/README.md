# Simple Offline Indoor Weather Station
Required hardware: Adafruit NRF52840 Feather Sense board and 3.5" TFT Featherwing.  

I'm using this as a basic temperature, humidity, and barometric pressure display inside my garage. In Florida, on July 1st, it's 95F in here. :/

This is all offline real-time sensor data. The demo shows all 9 of the Feather Sense's onboard sensors for the purpose of a beginner demo.

 ![](https://raw.githubusercontent.com/DJDevon3/Arduino/master/Adafruit%20NRF52840%20Feather%20Sense/DJDevon3_Simple_Offline_Weatherstation.jpg)
 
Once you play around with the demo you'll see how easy it is to customize it using setCursor positions for text and variables. Customize to your hearts content.

 ![](https://raw.githubusercontent.com/DJDevon3/Arduino/master/Adafruit%20NRF52840%20Feather%20Sense/DJDevon3_MyCustom_Offline_Weatherstation.jpg)
 
 After some customization using setCursor positioning, temp sensor buffer, and slightly improved code. The only sensor that is +-1% is the temp sensor. All the other sensors on the Feather Sense board range from wildly innacurate to somewhat innacurate in my climate zone.
 
  ![](https://github.com/DJDevon3/Arduino/blob/master/Adafruit%20NRF52840%20Feather%20Sense/DJDevon3_MyCustom_Offline_Weatherstation_Humidity.jpg)
