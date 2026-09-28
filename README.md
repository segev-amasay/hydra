# Project Hydra

Project Hydra is an IoT device incorporating an ESP8266 board, temperature and humidity sensor, and an LCD display. It shows time, temperature, humidity, and Wi-Fi details with minimal user input.

Project Hydra is built primarily around an ESP8266 NodeMCU development board, designed to provide environmental tracking and real-time data with minimal user input. Programmed in C++ via the Arduino framework, the device synchronizes time over Wi-Fi with an NTP server and polls a DHT11 sensor every three seconds to capture ambient temperature and relative humidity readings.

The hardware is assembled on a standard breadboard and incorporates a 1602 LCD module over I2C along with three tactile pushbuttons that cycle between display views. Users can toggle through three dedicated screens: the synchronized date and 24-hour time, the live temperature (in °C/°F) and humidity levels, and the active network status showing the connected Wi-Fi SSID and local IP address.

Read more here: https://segevamasay.wordpress.com/2026/02/07/project-hydra/
