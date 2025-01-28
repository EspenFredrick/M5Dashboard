// globals.h
#pragma once
#include <M5EPD.h>
#include <WiFi.h>
#include <TimeLib.h>
#include <time.h>

// Forward declarations of all global variables
extern M5EPD_Canvas canvas;
extern M5EPD_Canvas statusBar;

// WLAN
extern String currentSSID;
extern String currentPassword;
extern bool isWiFiConnected;

// Page states
enum Page {
    MAIN_MENU,
    CLOCK,
    CALENDAR,
    NOTES,
    SETTINGS,
    WLAN_SETTINGS,
    POWER_SETTINGS,
    FILES
};
extern Page currentPage;

// Constants
const int SQUIRCLE_SIZE = 160;
const int GRID_COLS = 4;
const int GRID_ROWS = 2;
const int MARGIN = 65;
const int TOP_MARGIN = 10;
const int LABEL_MARGIN = 15;

// Time sync
extern const char* ntpServer;
extern const long gmtOffset_sec;
extern const int daylightOffset_sec;

// Power management
extern const int LOW_BATTERY_THRESHOLD;
extern const unsigned long INACTIVITY_TIMEOUT;
extern unsigned long lastActivityTime;
extern bool isLowPowerMode;

// Reboot and sleep
extern const char* powerItems[];
extern const int POWER_ITEMS_COUNT;

// Menu items
extern const char* labels[];
extern const char* settingsItems[];
extern const int SETTINGS_ITEMS_COUNT;
extern int selectedSettingItem;
extern const char* notesItems[];
extern const int NOTES_ITEMS_COUNT;
extern int selectedNotesItem;

// Files handling
extern String currentPath;
extern const int MAX_FILE_BUFFER;

// Square structure
struct Square {
    int x;
    int y;
};
extern Square squares[];
extern bool squircleFilled[];
