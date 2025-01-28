// network.h
#pragma once
#include "globals.h"

void syncTimeFromNTP();
void checkWiFiStatus();
bool connectToWiFi(const String& ssid, const String& password);