// network.cpp
#include "network.h"

void checkWiFiStatus() {
    if (WiFi.status() != WL_CONNECTED) {
        isWiFiConnected = false;
    }
}

bool connectToWiFi(const String& ssid, const String& password) {
    WiFi.begin(ssid.c_str(), password.c_str());
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(500);
        attempts++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        isWiFiConnected = true;
        syncTimeFromNTP();
        return true;
    }

    isWiFiConnected = false;
    return false;
}

void syncTimeFromNTP() {
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}
