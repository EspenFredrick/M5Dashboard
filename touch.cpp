// touch.cpp
#include "touch.h"
#include "ui.h"
#include "network.h"
#include "power.h"
#include "globals.h"

bool checkTouch(int touchX, int touchY, int squareX, int squareY) {
    return (touchX >= squareX && touchX <= squareX + SQUIRCLE_SIZE &&
            touchY >= squareY && touchY <= squareY + SQUIRCLE_SIZE);
}

void handleSettingsTouch(int touchX, int touchY) {
    if(touchY >= 90 && touchY <= 270) {
        int item = (touchY - 90) / 60;
        if(item < SETTINGS_ITEMS_COUNT) {
            selectedSettingItem = item;
            drawSettingsPage();
            delay(200);
            selectedSettingItem = -1;
            if(item == 0) {
                currentPage = WLAN_SETTINGS;
                drawScreen();
            } else if(item == 2) {  // Assuming Power is the third item
                currentPage = POWER_SETTINGS;
                drawScreen();
            }
        }
    }
}

void handleWLANTouch(int touchX, int touchY) {
    // Check SSID field touch
    if (touchX >= 40 && touchX <= 440 && touchY >= 140 && touchY <= 190) {
        // Here you would implement keyboard input for SSID
        currentSSID = "Milkyway24";
        drawWLANPage();
    }
    // Check password field touch
    else if (touchX >= 40 && touchX <= 440 && touchY >= 260 && touchY <= 310) {
        // Here you would implement keyboard input for password
        currentPassword = "Fs*tYp%s37s;?";
        drawWLANPage();
    }
    // Check submit button touch
    else if (touchX >= 140 && touchX <= 340 && touchY >= 340 && touchY <= 400) {
        if (connectToWiFi(currentSSID, currentPassword)) {
            isWiFiConnected = true;
        } else {
            isWiFiConnected = false;
        }
        drawWLANPage();
    }
}

void handlePowerPageTouch(int touchX, int touchY) {
    if(touchY >= 90 && touchY <= 270) {
        int item = (touchY - 90) / 60;
        if(item < POWER_ITEMS_COUNT) {
            selectedSettingItem = item;
            drawPowerPage();
            delay(200);
            selectedSettingItem = -1;

            switch(item) {
                case 0:
                    goToSleep();
                    break;
                case 1:
                    rebootDevice();
                    break;
            }
        }
    }
}

void handleTouch() {
    if (M5.TP.available()) {
        lastActivityTime = millis();  // Reset inactivity timer on any touch
        M5.TP.update();

        if(M5.TP.getFingerNum() == 2) {
            if(currentPage != MAIN_MENU) {
                currentPage = MAIN_MENU;
                drawScreen();
                delay(300);
                return;
            }
        }

        if (!M5.TP.isFingerUp()) {
            tp_finger_t FingerItem = M5.TP.readFinger(0);
            int touchX = FingerItem.x;
            int touchY = FingerItem.y - 50;

            switch(currentPage) {
                case MAIN_MENU:
                    for (int i = 0; i < GRID_ROWS * GRID_COLS; i++) {
                        if (checkTouch(touchX, touchY, squares[i].x, squares[i].y)) {
                            if(i == 0) {
                                currentPage = CLOCK;
                                drawScreen();
                            } else if(i == 2) {
                                currentPage = CALENDAR;
                                drawScreen();
                            } else if(i == 3) {
                                currentPage = NOTES;
                                drawScreen();
                            } else if(i == 4) {
                                drawScreen();
                            } else if(i == 5) {
                                drawScreen();
                            } else if(i == 6) {
                                currentPage = FILES;
                                drawScreen();
                            } else if(i == 7) {
                                currentPage = SETTINGS;
                                drawScreen();
                            } else {
                                drawScreen();
                            }
                            delay(300);
                            break;
                        }
                    }
                    break;
                case CLOCK:
                    break;
                    
                case CALENDAR:
                    break;

                case NOTES:
                    if(touchY >= 90 && touchY <= 130 && touchX >= 40 && touchX <= 920) {
                        // Handle new note creation
                    }
                    break;

                case FILES:
                    if (touchY >= 90 && touchY <= 500) {
                        int item = (touchY - 90) / 40;
                        File root = SD.open(currentPath);
                        if (root) {
                            // Handle back button
                            if (currentPath != "/" && item == 0) {
                                currentPath = currentPath.substring(0, currentPath.lastIndexOf('/', currentPath.length() - 2) + 1);
                                drawFilesPage();
                                return;
                            }

                            // Skip items to get to selected file
                            int currentItem = currentPath != "/" ? 1 : 0;  // Skip back button if not in root
                            File file = root.openNextFile();
                            while (file && currentItem < item) {
                                file = root.openNextFile();
                                currentItem++;
                            }

                            if (file) {
                                if (file.isDirectory()) {
                                    currentPath = String(file.path()) + "/";
                                    drawFilesPage();
                                } else {
                                    openFile(file.path());
                                }
                            }
                            root.close();
                        }
                    }
                    break;

                case SETTINGS:
                    handleSettingsTouch(touchX, touchY);
                    break;

                case WLAN_SETTINGS:
                    handleWLANTouch(touchX, touchY);
                    break;

                case POWER_SETTINGS:
                    handlePowerPageTouch(touchX, touchY);
                    break;
            }
        }
    }
}