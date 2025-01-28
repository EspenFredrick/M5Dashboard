#pragma once
#include "globals.h"
#include "power.h"  // for getBatteryPercentage()
#include "network.h" // for isWiFiConnected

extern String currentSSID;

void drawStatusBar();