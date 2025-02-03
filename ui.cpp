#include "globals.h"
// ui.cpp
#include "ui.h"
#include "statusbar.h"

// In ui.cpp
void drawSquircle(int x, int y, const char* label, bool filled, int index) {
    squares[index] = {x, y};

    // Draw the squircle outline or fill
    if (filled) {
        canvas.fillRoundRect(x, y, SQUIRCLE_SIZE, SQUIRCLE_SIZE, 30, 15);
    } else {
        canvas.drawRoundRect(x, y, SQUIRCLE_SIZE, SQUIRCLE_SIZE, 30, 15);
    }

    // Load and draw the corresponding PNG icon
    String iconPath = "/icons/";
    iconPath += labels[index];
    iconPath.toLowerCase();
    iconPath += ".png";

    canvas.drawPngFile(SD, iconPath.c_str(), x, y);

    // Draw the label
    int labelWidth = canvas.textWidth(label);
    int labelX = x + (SQUIRCLE_SIZE - labelWidth) / 2;
    int labelY = y + SQUIRCLE_SIZE + LABEL_MARGIN;
    canvas.drawString(label, labelX, labelY);
}

void drawMainMenu() {
    M5.EPD.Clear(true);
    canvas.fillCanvas(0);
    int startX = MARGIN;
    int startY = TOP_MARGIN;
    int labelIndex = 0;
    for(int row = 0; row < GRID_ROWS; row++) {
        for(int col = 0; col < GRID_COLS; col++) {
            int x = startX + col * (SQUIRCLE_SIZE + MARGIN);
            int y = startY + row * (SQUIRCLE_SIZE + MARGIN + 40);
            drawSquircle(x, y, labels[labelIndex], squircleFilled[labelIndex], labelIndex);
            labelIndex++;
        }
    }
    drawStatusBar();  // Draw status bar after main canvas
    canvas.pushCanvas(0, 50, UPDATE_MODE_DU4);  // Push main canvas at y=50
}

void drawClock() {
    canvas.fillCanvas(0);

    struct tm timeinfo;
    canvas.setTextSize(16);  // Keep the huge size
    if (getLocalTime(&timeinfo)) {
        char timeStr[12];
        int hour12 = timeinfo.tm_hour % 12;
        if (hour12 == 0) hour12 = 12;
        sprintf(timeStr, "%d:%02d %s", 
                hour12,
                timeinfo.tm_min,
                timeinfo.tm_hour >= 12 ? "PM" : "AM");

        // Center calculation
        int textWidth = canvas.textWidth(timeStr);
        int textHeight = 16 * 16;  // Approximate height based on text size

        // Center horizontally (screen width is 960)
        int x = (960 - textWidth) / 2;
        // Center vertically (screen height is 540)
        int y = (540 - textHeight) / 2;

        canvas.drawString(timeStr, x, y);
    } else {
        // Center the error message too
        const char* errorMsg = "--:-- --";
        int textWidth = canvas.textWidth(errorMsg);
        int textHeight = 16 * 16;
        int x = (960 - textWidth) / 2;
        int y = (540 - textHeight) / 2;
        canvas.drawString(errorMsg, x, y);
    }

    canvas.setTextSize(3);  // Reset text size
    canvas.pushCanvas(0, 0, UPDATE_MODE_DU4);
}

void drawCalendar() {
    canvas.fillCanvas(0);

    // Get current time
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        // If time sync failed, set to Jan 1st 2100
        timeinfo.tm_year = 200;  // Years since 1900
        timeinfo.tm_mon = 0;     // January (0-11)
        timeinfo.tm_mday = 1;    // 1st day
        timeinfo.tm_wday = 5;    // Saturday (Jan 1st 2100 is a Saturday)
    }

    // Month name array
    const char* months[] = {
        "January", "February", "March", "April", "May", "June",
        "July", "August", "September", "October", "November", "December"
    };

    // Day name array
    const char* weekdays[] = {
        "Sunday", "Monday", "Tuesday", "Wednesday", 
        "Thursday", "Friday", "Saturday"
    };

    // Left side - Month and Day
    canvas.setTextSize(6);
    char headerStr[50];
    sprintf(headerStr, "%s %d", months[timeinfo.tm_mon], timeinfo.tm_mday);
    canvas.drawString(headerStr, 20, 20);

    canvas.setTextSize(4);
    canvas.drawString(weekdays[timeinfo.tm_wday], 20, 70);

    // Right side - Calendar Grid
    const int CALENDAR_START_X = 500;
    const int CALENDAR_START_Y = 30;
    const int CELL_WIDTH = 60;
    const int CELL_HEIGHT = 60;
    const int HEADER_HEIGHT = 40;

    // Draw day labels
    canvas.setTextSize(2);
    const char* days[] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};
    for (int i = 0; i < 7; i++) {
        int x = CALENDAR_START_X + (i * CELL_WIDTH);
        canvas.drawString(days[i], x + 5, CALENDAR_START_Y);
    }

    // Draw calendar grid
    for (int row = 0; row < 6; row++) {
        for (int col = 0; col < 7; col++) {
            int x = CALENDAR_START_X + (col * CELL_WIDTH);
            int y = CALENDAR_START_Y + HEADER_HEIGHT + (row * CELL_HEIGHT);
            canvas.drawRect(x, y, CELL_WIDTH, CELL_HEIGHT, 15);
        }
    }

    canvas.pushCanvas(0, 50, UPDATE_MODE_DU4);
}

void drawNotesPage() {
    canvas.fillCanvas(0);
    canvas.setTextSize(4);
    canvas.drawString("Notes", 20, 20);

    canvas.setTextSize(3);
    // Draw the "+ New Quick Note" at the top
    canvas.drawString(notesItems[0], 40, 100);

    // Draw a line under the new note option
    canvas.drawFastHLine(40, 140, 880, 15);

    canvas.pushCanvas(0, 50, UPDATE_MODE_DU4);
}

void drawSettingsPage() {
    canvas.fillCanvas(0);
    canvas.setTextSize(4);
    canvas.drawString("Settings", 20, 20);

    canvas.setTextSize(3);
    for(int i = 0; i < SETTINGS_ITEMS_COUNT; i++) {
        int y = 100 + (i * 60);
        if(i == selectedSettingItem) {
            canvas.fillRect(0, y - 10, 960, 50, 15);
            canvas.setTextColor(0);
        }
        canvas.drawString(settingsItems[i], 40, y);
        canvas.setTextColor(15);
    }
    canvas.pushCanvas(0, 50, UPDATE_MODE_DU4);
}

void drawWLANPage() {
    canvas.fillCanvas(0);
    canvas.setTextSize(4);
    canvas.drawString("WLAN", 20, 20);

    // Left side - Input fields
    canvas.setTextSize(3);
    canvas.drawString("Network SSID:", 40, 100);
    canvas.drawRect(40, 140, 400, 50, 15);
    canvas.setTextSize(2);
    canvas.drawString(currentSSID, 50, 155);

    canvas.setTextSize(3);
    canvas.drawString("Password:", 40, 220);
    canvas.drawRect(40, 260, 400, 50, 15);
    canvas.setTextSize(2);
    canvas.drawString("********", 50, 275);

    // Submit button
    canvas.drawRect(140, 340, 200, 60, 15);
    canvas.setTextSize(3);
    canvas.drawString("Submit", 190, 355);

    // Right side - WLAN Info
    const int RIGHT_START_X = 520;
    canvas.setTextSize(4);
    canvas.drawString("WLAN Info", RIGHT_START_X, 100);

    canvas.setTextSize(3);
    // SSID line
    canvas.drawString("SSID:", RIGHT_START_X, 180);
    int ssidLabelWidth = canvas.textWidth("SSID:");
    canvas.drawString(isWiFiConnected ? currentSSID : "Not Connected", 
                    RIGHT_START_X + ssidLabelWidth + 20, 180);

    // Status line
    canvas.drawString("Status:", RIGHT_START_X, 220);
    int statusLabelWidth = canvas.textWidth("Status:");
    canvas.drawString(isWiFiConnected ? "Connected" : "Not Connected", 
                    RIGHT_START_X + statusLabelWidth + 20, 220);

    canvas.pushCanvas(0, 50, UPDATE_MODE_DU4);
}

void drawPowerPage() {
    canvas.fillCanvas(0);
    canvas.setTextSize(4);
    canvas.drawString("Power", 20, 20);

    canvas.setTextSize(3);
    for(int i = 0; i < POWER_ITEMS_COUNT; i++) {
        int y = 100 + (i * 60);
        if(i == selectedSettingItem) {
            canvas.fillRect(0, y - 10, 960, 50, 15);
            canvas.setTextColor(0);
        }
        canvas.drawString(powerItems[i], 40, y);
        canvas.setTextColor(15);
    }
    canvas.pushCanvas(0, 50, UPDATE_MODE_DU4);
}

//- Files functions ---------------------------------------------------------------------------------------------
void openFile(const char* path) {
    String extension = String(path).substring(String(path).lastIndexOf('.'));
    extension.toLowerCase();

    if (extension == ".txt") {
        renderTextFile(path);
    } else if (extension == ".png" || extension == ".jpg" || extension == ".bmp") {
        renderImageFile(path);
    }
}

void renderImageFile(const char* path) {
    canvas.fillCanvas(0);
    canvas.drawPngFile(SD, path, 0, 50);  // Adjust position as needed
    canvas.pushCanvas(0, 50, UPDATE_MODE_DU4);
}
void renderTextFile(const char* path) {
    File file = SD.open(path);
    if (!file) return;

    canvas.fillCanvas(0);
    canvas.setTextSize(3);

    int yPos = 100;
    String line;
    while (file.available() && yPos < 500) {
        char c = file.read();
        if (c == '\n') {
            canvas.drawString(line, 40, yPos);
            yPos += 40;
            line = "";
        } else {
            line += c;
        }

        // Check if line is too long for screen
        if (canvas.textWidth(line.c_str()) > 880) {
            canvas.drawString(line, 40, yPos);
            yPos += 40;
            line = "";
        }
    }

    // Draw any remaining text
    if (line.length() > 0) {
        canvas.drawString(line, 40, yPos);
    }

    file.close();
    canvas.pushCanvas(0, 50, UPDATE_MODE_DU4);
}

void drawFilesPage() {
    canvas.fillCanvas(0);
    canvas.setTextSize(4);
    canvas.drawString("Files", 20, 20);

    if (!SD.begin()) {
        canvas.setTextSize(3);
        canvas.drawString("No SD card inserted.", 40, 100);
    } else {
        File root = SD.open(currentPath);
        if (!root) {
            canvas.setTextSize(3);
            canvas.drawString("Error opening directory", 40, 100);
        } else {
            canvas.setTextSize(3);
            int yPos = 100;

            // Add back button if not in root
            if (currentPath != "/") {
                canvas.drawString("< Back", 40, yPos);
                yPos += 40;
            }

            File file = root.openNextFile();
            while(file && yPos < 500) {
                // Draw folder/file icon
                if (file.isDirectory()) {
                    canvas.drawString("📁", 40, yPos);
                } else {
                    canvas.drawString("📄", 40, yPos);
                }

                // Draw filename
                canvas.drawString(file.name(), 80, yPos);

                // Draw file size on right (only for files)
                if (!file.isDirectory()) {
                    String fileSize;
                    float size = file.size();
                    if (size < 1024) {
                        fileSize = String(size) + " B";
                    } else if (size < 1024 * 1024) {
                        fileSize = String(size / 1024.0, 1) + " KB";
                    } else {
                        fileSize = String(size / (1024.0 * 1024.0), 1) + " MB";
                    }

                    int rightX = 920 - canvas.textWidth(fileSize.c_str());
                    canvas.drawString(fileSize.c_str(), rightX, yPos);
                }

                yPos += 40;
                file = root.openNextFile();
            }
            root.close();
        }
    }
    canvas.pushCanvas(0, 50, UPDATE_MODE_DU4);
}
//---------------------------------------------------------------------------------------------------------

void drawScreen() {
    switch(currentPage) {
        case MAIN_MENU:
            drawMainMenu();
            break;
        case CLOCK:
            drawClock();
            break;
        case CALENDAR:
            drawCalendar();
            break;
        case NOTES:
            drawNotesPage();
            break;
        case SETTINGS:
            drawSettingsPage();
            break;
        case WLAN_SETTINGS:
            drawWLANPage();
            break;
        case POWER_SETTINGS:
            drawPowerPage();
            break;
        case FILES:
            drawFilesPage();
            break;
    }
    drawStatusBar();
}
