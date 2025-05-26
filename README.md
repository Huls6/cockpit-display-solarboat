# cockpit-display-solarboat

This code is written for project display-cockpit of the HAN Solarboat. 
For this project there is a new display designed for in the cockpit of the boat. 
The microcontroller used for this display is:

LilyGO TTGO T-SIM7000G ESP32-WROVER - 4MB - met 18650 Batterijhouder
Link to TinyTronics: https://www.tinytronics.nl/nl/development-boards/microcontroller-boards/met-telecommunicatie/lilygo-t-sim7000g-esp32-wrover-16mb-met-18650-batterijhouder

To use the software the following is needed:
-Espressif environment
-Jetbrains Clion
-Setup environment to combine the two software packages
See the following links to do this:
1.	Download ESP environment: https://youtu.be/wDi58Najcs8?si=PeqcL4uU1XhrIbV4 
2.	How to use ESP-IDF: https://youtu.be/dOVjb2wXI84?si=OW72HJmEmQ_OVxx2
3.	Clion configuration: https://youtu.be/M6fa7tzZdLw?si=waKibBgPJOQm9xFS

To configurate the code see config.h, the following can be changed:
```cpp
#define DISPLAY_UPSIDEDOWN 	// Uncomment to reverse display screen
#define DEBUGMODE   		// Print info to UART0, uncomment to disable
#define SIMPIN  "AT+CPIN=00000"	// Pincode of the SIM
#define samples 3		//Moving average sample moments, for smoothing out the power consumption value
```

Code functionality in general:
-Drive the display.
-Read CAN-bus.
-Parse CAN-bus data to useful information to show on the display.
-GPS functionality, for location and speed. Extended by adding option Xtra, similar to A-GPS where a faster fix is realised by calculating the satellites position over LTE.
-LTE functionality, to log data to the cloud and have internet access.
-Log data to an SQL-server. This server runs Grafana with a dashboard for data-analysing.
