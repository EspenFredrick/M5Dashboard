#include <M5EPD.h>
#include <WiFi.h>
#include <TimeLib.h>
#include <time.h>

M5EPD_Canvas canvas(&M5.EPD);
M5EPD_Canvas statusBar(&M5.EPD);

// WLAN credentials
String currentSSID = "Milkyway24";  // Placeholder SSID
String currentPassword = "Fs*tYp%s37s;?";  // Placeholder password
bool isWiFiConnected = false;  // Connection status

// Page states
enum Page {
    MAIN_MENU,
    CALENDAR,
    NOTES,
    SETTINGS,
    WLAN_SETTINGS
};

Page currentPage = MAIN_MENU;

const int SQUIRCLE_SIZE = 160;
const int GRID_COLS = 4;
const int GRID_ROWS = 2;
const int MARGIN = 65;
const int TOP_MARGIN = 10;
const int LABEL_MARGIN = 15;

const char* labels[] = {
    "Clock", "Weather", "Calendar", "Notes", 
    "Astronomy", "News", "Files", "Settings"
};


bool connectToWiFi(const String& ssid, const String& password) {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), password.c_str());

    // Wait up to 10 seconds for connection
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(500);
        attempts++;
    }

    isWiFiConnected = (WiFi.status() == WL_CONNECTED);
    return isWiFiConnected;
}

bool squircleFilled[GRID_ROWS * GRID_COLS] = {false};
struct Square {
    int x;
    int y;
} squares[GRID_ROWS * GRID_COLS];

// Settings menu items
const char* settingsItems[] = {"WLAN", "Time and Date", "Misc"};
const int SETTINGS_ITEMS_COUNT = 3;
int selectedSettingItem = -1;

// Notes items
const char* notesItems[] = {"+ New Quick Note"};
const int NOTES_ITEMS_COUNT = 1;
int selectedNotesItem = -1;

// Forward declarations
void drawScreen();
void drawMainMenu();
void drawCalendar();
void drawNotesPage();
void drawSettingsPage();
void drawWLANPage();
void drawStatusBar();
void handleSettingsTouch(int touchX, int touchY);
bool checkTouch(int touchX, int touchY, int squareX, int squareY);

void setup() {
    M5.begin(true, false, true, true, false);
    M5.EPD.SetRotation(0);
    M5.EPD.Clear(true);
    M5.TP.SetRotation(0);
    canvas.createCanvas(960, 540);
    statusBar.createCanvas(960, 40);
    canvas.setTextSize(3);
    statusBar.setTextSize(2);
    drawScreen();
}

void drawStatusBar() {
    statusBar.fillCanvas(0);
    statusBar.drawString("12:00 PM", 10, 10);
    statusBar.drawString("BATTERY", 860, 10);
    statusBar.drawString("SSID:", 750, 10);
    statusBar.pushCanvas(0, 0, UPDATE_MODE_DU4);
}

void drawSquircle(int x, int y, const char* label, bool filled, int index) {
    squares[index] = {x, y};
    if (filled) {
        canvas.fillRoundRect(x, y, SQUIRCLE_SIZE, SQUIRCLE_SIZE, 30, 15);
    } else {
        canvas.drawRoundRect(x, y, SQUIRCLE_SIZE, SQUIRCLE_SIZE, 30, 15);
    }
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
    canvas.pushCanvas(0, 50, UPDATE_MODE_DU4);
}

void drawCalendar() {
    canvas.fillCanvas(0);

    // Left side - Month and Day
    canvas.setTextSize(6);
    canvas.drawString("January", 20, 20);
    canvas.setTextSize(4);
    canvas.drawString("31 Monday", 20, 70);  // Placeholder date

    // Right side - Calendar Grid
    const int CALENDAR_START_X = 480;  // Start at middle of screen
    const int CALENDAR_START_Y = 20;
    const int CELL_WIDTH = 60;
    const int CELL_HEIGHT = 50;
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
    canvas.drawString(currentSSID, 50, 155);  // Display current SSID in field

    canvas.setTextSize(3);
    canvas.drawString("Password:", 40, 220);
    canvas.drawRect(40, 260, 400, 50, 15);
    canvas.setTextSize(2);
    canvas.drawString("********", 50, 275);  // Show asterisks for password

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

void drawScreen() {
    switch(currentPage) {
        case MAIN_MENU:
            drawMainMenu();
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
    }
    drawStatusBar();
}

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
            }
        }
    }
}

void handleTouch() {
    if (M5.TP.available()) {
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
                            if(i == 7) {
                                currentPage = SETTINGS;
                                drawScreen();
                            } else if(i == 3) {
                                currentPage = NOTES;
                                drawScreen();
                            } else if(i == 2) {
                                currentPage = CALENDAR;
                                drawScreen();
                            } else {
                                squircleFilled[i] = !squircleFilled[i];
                                drawScreen();
                            }
                            delay(300);
                            break;
                        }
                    }
                    break;
                case CALENDAR:
                    break;

                case NOTES:
                    if(touchY >= 90 && touchY <= 130 && touchX >= 40 && touchX <= 920) {
                        // Handle new note creation
                    }
                    break;

                case SETTINGS:
                    handleSettingsTouch(touchX, touchY);
                    break;
                
                case WLAN_SETTINGS:
                    if (!M5.TP.isFingerUp()) {
                        tp_finger_t FingerItem = M5.TP.readFinger(0);
                        int touchX = FingerItem.x;
                        int touchY = FingerItem.y - 50;

                        // Check SSID field touch
                        if (touchX >= 40 && touchX <= 440 && touchY >= 140 && touchY <= 190) {
                            // Here you would implement keyboard input for SSID
                            // For now, we'll use the placeholder value
                            currentSSID = "Milkyway24";
                            drawWLANPage();
                        }
                        // Check password field touch
                        else if (touchX >= 40 && touchX <= 440 && touchY >= 260 && touchY <= 310) {
                            // Here you would implement keyboard input for password
                            // For now, we'll use the placeholder value
                            currentPassword = "Fs*tYp%s37s;?";
                            drawWLANPage();
                        }
                        // Check submit button touch
                        else if (touchX >= 140 && touchX <= 340 && touchY >= 340 && touchY <= 400) {
                            // Try to connect to WiFi
                            if (connectToWiFi(currentSSID, currentPassword)) {
                                isWiFiConnected = true;
                            } else {
                                isWiFiConnected = false;
                            }
                            drawWLANPage();
                        }
                    }
                    break;

            }
        }
    }
}

void loop() {
    M5.update();
    if (M5.BtnP.wasPressed()) {
        M5.EPD.Clear(true);
        drawScreen();
        delay(500);
    }

    // Check WiFi status every 5 seconds
    static unsigned long lastWiFiCheck = 0;
    if (millis() - lastWiFiCheck >= 5000) {
        checkWiFiStatus();
        if (currentPage == WLAN_SETTINGS) {
            drawWLANPage();  // Update the display if we're on the WLAN page
        }
        lastWiFiCheck = millis();
    }

    handleTouch();
    delay(10);
}

void checkWiFiStatus() {
    if (WiFi.status() != WL_CONNECTED) {
        isWiFiConnected = false;
    } else {
        isWiFiConnected = true;
    }
}

