// ui.h
#pragma once
#include "globals.h"
#include "network.h" // for isWiFiConnected

void drawScreen();
void drawSquircle(int x, int y, const char* label, bool filled, int index);

// Home screen functions
void drawClock();
void drawMainMenu();
void drawCalendar();
void drawNotesPage();
void drawSettingsPage();

// Settings functions
void drawWLANPage();
void drawPowerPage();

// Files page functions
void drawFilesPage();
void openFile(const char* path);
void renderTextFile(const char* path);
void renderImageFile(const char* path);
