// globals.cpp
#include "globals.h"

M5EPD_Canvas canvas(&M5.EPD);
M5EPD_Canvas statusBar(&M5.EPD);

// WLAN
String currentSSID = "Milkyway24";
String currentPassword = "Fs*tYp%s37s;?";
bool isWiFiConnected = false;

// Time sync
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = -21600;
const int daylightOffset_sec = 3600;

// Power management
const int LOW_BATTERY_THRESHOLD = 15;
const unsigned long INACTIVITY_TIMEOUT = 1 * 60 * 1000; // Change the first number to change the timeout
unsigned long lastActivityTime = 0;
bool isLowPowerMode = false;

// Reboot and sleep
const char* powerItems[] = {
    "Sleep Mode",
    "Reboot Device"
};
const int POWER_ITEMS_COUNT = 2;

// Menu items
const char* labels[] = {
    "Clock", "Weather", "Calendar", "Notes", 
    "Astronomy", "News", "Files", "Settings"
};

// Settings items
const char* settingsItems[] = {"WLAN", "Time and Date", "Power", "Misc"};
const int SETTINGS_ITEMS_COUNT = 4;
int selectedSettingItem = -1;

// Notes items
const char* notesItems[] = {"+ New Quick Note"};
const int NOTES_ITEMS_COUNT = 1;
int selectedNotesItem = -1;

// Files items
String currentPath = "/";
Page currentPage = MAIN_MENU;
const int MAX_FILE_BUFFER = 32768;

Square squares[GRID_ROWS * GRID_COLS];
bool squircleFilled[GRID_ROWS * GRID_COLS] = {false};
