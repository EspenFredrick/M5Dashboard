// touch.h
#pragma once
#include "globals.h"

bool checkTouch(int touchX, int touchY, int squareX, int squareY);
void handleSettingsTouch(int touchX, int touchY);
void handleWLANTouch(int touchX, int touchY);
void handleTouch();
void handlePowerPageTouch(int touchX, int touchY);