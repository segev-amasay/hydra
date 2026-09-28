// default include
#include <Arduino.h>
#include <time.h>
#include <sntp.h>

// LCD library
#include <LiquidCrystal_I2C.h>

// WiFi library
#include <ESP8266WiFi.h>

// DHT11 library
#include <DHT.h>

// DHT11 config
#define DHTPIN D3
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// WiFi credentials (change before running!)
const char* ssid     = "changeme";
const char* password = "changeme";
const char* hostname = "changeme";

// define LCD object (LCD1602)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// 3 buttons
const int buttonA = D5; // time
const int buttonB = D6; // temp/humidity
const int buttonC = D7; // WiFi info

// NTP server configuration (sntp.h)
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = -14400; // UTC -5: 18000 seconds // UTC -4: 14400 seconds
const int   daylightOffset_sec = 0;

// Time display (global)
time_t now = time(nullptr);
struct tm* t = localtime(&now);

// Screens
enum ScreenMode {
    MODE_WELCOME,
    MODE_TIME,
    MODE_SENSOR,
    MODE_WIFI
};

// Startup
ScreenMode currentMode = MODE_WELCOME;
int welcomeScreens = 0;
unsigned long previousMillis = 0;
unsigned long refreshInterval = 0;

unsigned long serialPrintInterval = 0;

// Screen update fxn prototype
void updateScreen();

void welcomeScreen(); // welcome screen
void displayTime();   // button A
void displayTemp();   // button B
void displayWifi();   // button C

void dataPrint();    // serial data print

// --------------------------------------------------------------

void setup() {
    pinMode(buttonA, INPUT_PULLUP);
    pinMode(buttonB, INPUT_PULLUP);
    pinMode(buttonC, INPUT_PULLUP);
    
    Serial.begin(9600);

    lcd.init();
    lcd.backlight();
    lcd.clear();

    // Boot screen 1
    lcd.setCursor(0, 0);
    lcd.print(" PROJECT HYDRA  ");
    lcd.setCursor(0, 1);
    lcd.print("  Version 1.0   ");

    delay(3000);

    lcd.clear();

    // Boot screen 2
    lcd.setCursor(0, 0);
    lcd.print("  Starting...  ");

    // Serial.begin(9600);

    delay(4000); // serial initialization delay

    Serial.println();

    Serial.println("Project Hydra: Digital Clock with Temperature Monitoring");
    Serial.println("Version 1.0");
    Serial.println("Powered by ESP8266 NodeMCU 1.0 (ESP-12E)");
    Serial.println("Starting...");

    Serial.println();

    Serial.print("Connecting to network: ");
    Serial.print(ssid);
    Serial.print("...");

    WiFi.hostname(hostname);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(200);
        Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println(" connected!");
        Serial.print("Local IP address: ");
        Serial.println(WiFi.localIP());
        Serial.println("Your hostname is " + WiFi.hostname());
    }

    Serial.println();

    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    
    dht.begin();

    lcd.clear();

    currentMode = MODE_WELCOME;
    refreshInterval = 3000;
    updateScreen();

    // Main menu
    lcd.setCursor(0, 0);
    lcd.print("    Welcome     ");

}

// --------------------------------------------------------------

void loop() {
    unsigned long currentMillis = millis();

    // Reinserted for serial printing?
    time_t now = time(nullptr);
    struct tm* t = localtime(&now); // comment line to test offline condition

    if (currentMillis - serialPrintInterval >= 1000) { // every 10 seconds
        serialPrintInterval = currentMillis;
            dataPrint();
        }

    // Button states
    int stateA = digitalRead(buttonA);
    int stateB = digitalRead(buttonB);
    int stateC = digitalRead(buttonC);
    
    // Button A: Time display
    if (stateA == LOW) {
        currentMode = MODE_TIME;
        refreshInterval = 1000; 
        updateScreen(); 
        previousMillis = currentMillis; 
        delay(200);
    }

    // Button B: Temp/Humidity display
    else if (stateB == LOW) {
        currentMode = MODE_SENSOR;
        refreshInterval = 3000;
        updateScreen();
        previousMillis = currentMillis;
        delay(200);
    }

    // Button C: WiFi info display
    else if (stateC == LOW) {
        currentMode = MODE_WIFI;
        refreshInterval = 5000; 
        updateScreen();
        previousMillis = currentMillis;
        delay(200);
    }

    // Auto screen refresh (time, sensor, wifi)
    if (currentMillis - previousMillis >= refreshInterval) {
            previousMillis = currentMillis;
            updateScreen();
        }
}

// --------------------------------------------------------------

void updateScreen() {
    switch (currentMode) {
        case MODE_TIME:
            displayTime();
            break;
        case MODE_SENSOR:
            displayTemp();
            break;
        case MODE_WIFI:
            displayWifi();
            break;
        case MODE_WELCOME:
            welcomeScreen();
            break;
    }
}

void displayTime() {
    if (t->tm_year > (2020 - 1900)) { 
        lcd.setCursor(0, 0);
        lcd.printf("%04d-%02d-%02d      ", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday);
        lcd.setCursor(0, 1);
        lcd.printf("%02d:%02d:%02d        ", t->tm_hour, t->tm_min, t->tm_sec);
    } else {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("- TIME OFFLINE - ");
    }
}

void displayTemp() {
    float humidity = dht.readHumidity();
    float temp_c = dht.readTemperature();
    float temp_f = dht.readTemperature(true);

    lcd.setCursor(0, 0);
    lcd.printf("%.2fC / %.2fF  ", temp_c, temp_f);
    lcd.setCursor(0, 1);
    lcd.printf("%.1f%% RH        ", humidity);

}

void displayWifi() {
    lcd.setCursor(0, 0);
    lcd.print(ssid);
    lcd.print("                ");
    lcd.setCursor(0, 1);
    lcd.print(WiFi.localIP());
    lcd.print("                ");
}

void welcomeScreen() {
    switch (welcomeScreens) {
        case 1:
            lcd.setCursor(0, 1);
            lcd.print("[A] Time & Date ");
            break;
        case 2:
            lcd.setCursor(0, 1);
            lcd.print("[B] Temp & Humid");
            break;
        case 3:
            lcd.setCursor(0, 1);
            lcd.print(" [C] WiFi Info  ");
            break;
    }

    welcomeScreens++;
    if (welcomeScreens > 3) {
        welcomeScreens = 1;
    }
}

void dataPrint() {
    Serial.print(ssid + String(" (") + WiFi.localIP().toString() + String(") - "));
    Serial.printf("%04d-%02d-%02d %02d:%02d:%02d - ", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
    Serial.printf("Temperature: %.2fC / %.2fF - Humidity: %.1f%%\n", dht.readTemperature(), dht.readTemperature(true), dht.readHumidity());
}