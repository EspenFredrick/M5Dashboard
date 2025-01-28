// power.cpp
#include "power.h"

int getBatteryPercentage() {
    return M5.getBatteryVoltage() * 100 / 4300;  // Assuming 4.3V is 100%
}

void goToSleep() {
    canvas.fillCanvas(0);
    canvas.setTextSize(4);
    canvas.drawString("Going to sleep...", 300, 250);
    canvas.pushCanvas(0, 0, UPDATE_MODE_DU4);
    delay(2000);

    if (isWiFiConnected) {
        WiFi.disconnect(true);
        WiFi.mode(WIFI_OFF);
    }

    esp_sleep_enable_ext0_wakeup(GPIO_NUM_38, LOW);  // Enable wakeup on power button
    esp_deep_sleep_start();
}

void rebootDevice() {
    canvas.fillCanvas(0);
    canvas.setTextSize(4);
    canvas.drawString("Rebooting...", 300, 250);
    canvas.pushCanvas(0, 0, UPDATE_MODE_DU4);
    delay(2000);

    if (isWiFiConnected) {
        WiFi.disconnect(true);
        WiFi.mode(WIFI_OFF);
    }

    ESP.restart();
}