#include "globals.h"
#include "power.h"
#include "network.h"
#include "ui.h"
#include "touch.h"
#include "statusbar.h"

#include <NimBLEDevice.h>
#include <NimBLEHIDDevice.h>
NimBLEHIDDevice* hidDevice;

void setup() {
    M5.begin(true, false, true, true, false);

    M5.EPD.SetRotation(0);
    M5.EPD.Clear(true);
    M5.TP.SetRotation(0);
    canvas.createCanvas(960, 540);
    statusBar.createCanvas(960, 40);
    canvas.setTextSize(3);
    statusBar.setTextSize(2);

    // Check wake up reason
    esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
    if (wakeup_reason == ESP_SLEEP_WAKEUP_EXT0 || wakeup_reason == ESP_SLEEP_WAKEUP_EXT1) {
        if (getBatteryPercentage() <= LOW_BATTERY_THRESHOLD) {
            goToSleep();
        }
    }

    lastActivityTime = millis();

    // Configure SD card pins for M5Paper
    SPI.begin(14, 13, 12, 4); // SCLK, MISO, MOSI, CS pins for M5Paper

    // Initialize SD Card with detailed error checking
    if (!SD.begin(4, SPI)) {  // Explicitly specify CS pin and SPI
        canvas.fillCanvas(0);
        canvas.setTextSize(2);

        // Check if card is present
        if (!SD.cardType()) {
            const char* errorMsg = "No SD Card detected! Check insertion";
            int textWidth = canvas.textWidth(errorMsg);
            int x = (960 - textWidth) / 2;
            canvas.drawString(errorMsg, x, 100);

            // Add hardware debug info
            String pinInfo = "CS:4 SCLK:14 MISO:13 MOSI:12";
            canvas.drawString(pinInfo.c_str(), (960 - canvas.textWidth(pinInfo.c_str())) / 2, 140);
        } else {
            const char* errorMsg = "SD Card Mount Failed - Check Format";
            int textWidth = canvas.textWidth(errorMsg);
            int x = (960 - textWidth) / 2;
            canvas.drawString(errorMsg, x, 100);
        }
        canvas.pushCanvas(0, 0, UPDATE_MODE_DU4);
        delay(2000);
    } else {
        canvas.fillCanvas(0);
        canvas.setTextSize(2);

        // Show successful mount with card info
        String cardInfo = "SD Card: ";
        cardInfo += String((float)SD.cardSize() / (1024 * 1024 * 1024), 1);
        cardInfo += "GB";

        int textWidth = canvas.textWidth(cardInfo.c_str());
        int x = (960 - textWidth) / 2;
        canvas.drawString(cardInfo.c_str(), x, 100);
        canvas.pushCanvas(0, 0, UPDATE_MODE_DU4);
        delay(2000);
    } 

    drawScreen();


    NimBLEDevice::init("M5Paper");
    NimBLEDevice::setSecurityAuth(true, true, true);
    hidDevice = NimBLEDevice::createHIDDevice();
    NimBLEDevice::start();
    Serial.println("Bluetooth HID Host initialized. Waiting for keyboard connection...");
}

void loop() {
    M5.update();
    static int lastMinute = -1;
    static int lastBatteryCheck = -1;
    static unsigned long lastBatteryCheckTime = 0;

    // Check for low battery or inactivity
   if (!isLowPowerMode) {
        int currentBattery = getBatteryPercentage();
        if (currentBattery <= LOW_BATTERY_THRESHOLD) {
            goToSleep();
        }

        // Check for inactivity
        unsigned long currentTime = millis();
        if (currentTime - lastActivityTime >= INACTIVITY_TIMEOUT) {
            goToSleep();
        }
    }

    // Reset activity timer if button is pressed
    if (M5.BtnP.wasPressed()) {
        lastActivityTime = millis();
        M5.EPD.Clear(true);
        drawScreen();
        delay(500);
    }

    // WiFi check
    static unsigned long lastWiFiCheck = 0;
    if (millis() - lastWiFiCheck >= 5000) {
        checkWiFiStatus();
        if (currentPage == WLAN_SETTINGS) {
            drawWLANPage();
        }
        lastWiFiCheck = millis();
    }

    // Update time display when minute changes
    if (isWiFiConnected) {
        struct tm timeinfo;
        if (getLocalTime(&timeinfo)) {
            if (timeinfo.tm_min != lastMinute) {
                drawStatusBar();
                lastMinute = timeinfo.tm_min;
                // Removed: lastActivityTime = millis();
            }
        }
    }

    // Check battery every 60 seconds
    if (millis() - lastBatteryCheckTime >= 60000) {
        int currentBatteryLevel = getBatteryPercentage();
        if (currentBatteryLevel != lastBatteryCheck) {
            drawStatusBar();
            lastBatteryCheck = currentBatteryLevel;
        }
        lastBatteryCheckTime = millis();
    }

    handleTouch();
    delay(10);
}