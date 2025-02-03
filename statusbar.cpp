// statusbar.cpp
#include "statusbar.h"
#include <time.h>

void drawStatusBar() {
    statusBar.fillCanvas(0);

    // Time display
    if (isWiFiConnected) {  // we'll need to include network.h or declare this extern
        struct tm timeinfo;
        if (getLocalTime(&timeinfo)) {
            char timeStr[12];
            int hour12 = timeinfo.tm_hour % 12;
            if (hour12 == 0) hour12 = 12;
            sprintf(timeStr, "%d:%02d %s", 
                    hour12,
                    timeinfo.tm_min,
                    timeinfo.tm_hour >= 12 ? "PM" : "AM");
            statusBar.drawString(timeStr, 10, 10);
        } else {
            statusBar.drawString("--:-- --", 10, 10);
        }
    } else {
        statusBar.drawString("--:-- --", 10, 10);
    }

    // SSID display
    if (isWiFiConnected) {
        statusBar.drawString(currentSSID, 700, 10);
    } else {
        statusBar.drawString("Not Connected", 700, 10);
    }

    // Battery status
    char batteryStr[8];
    sprintf(batteryStr, "%d%%", getBatteryPercentage());
    int batteryWidth = statusBar.textWidth(batteryStr);
    statusBar.drawString(batteryStr, 960 - batteryWidth - 10, 10);

    statusBar.pushCanvas(0, 0, UPDATE_MODE_DU4);
}
