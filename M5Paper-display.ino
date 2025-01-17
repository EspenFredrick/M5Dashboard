#include <M5EPD.h>

M5EPD_Canvas canvas(&M5.EPD);
M5EPD_Canvas statusBar(&M5.EPD);

const int SQUIRCLE_SIZE = 160;
const int GRID_COLS = 4;
const int GRID_ROWS = 2;
const int MARGIN = 65;
const int TOP_MARGIN = 10;
const int LABEL_MARGIN = 15;

const char* labels[] = {
    "Weather", "Calendar", "Cycling", "Clock",
    "Geospace", "Astronomy", "News", "Settings"
};

// Array to track which squares are filled
bool squircleFilled[GRID_ROWS * GRID_COLS] = {false};

// Structure to store square positions for touch detection
struct Square {
    int x;
    int y;
} squares[GRID_ROWS * GRID_COLS];

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
    statusBar.drawString("BATTERY", 850, 10);
    statusBar.drawString("WIFI", 750, 10);

    statusBar.pushCanvas(0, 0, UPDATE_MODE_DU4);
}

void drawSquircle(int x, int y, const char* label, bool filled, int index) {
    // Store square position for touch detection
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

void drawScreen() {
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
    drawStatusBar();
}

bool checkTouch(int touchX, int touchY, int squareX, int squareY) {
    return (touchX >= squareX && touchX <= squareX + SQUIRCLE_SIZE &&
            touchY >= squareY && touchY <= squareY + SQUIRCLE_SIZE);
}

void handleTouch() {
    if (M5.TP.available()) {
        M5.TP.update();

        if (!M5.TP.isFingerUp()) {
            tp_finger_t FingerItem = M5.TP.readFinger(0);
            int touchX = FingerItem.x;
            int touchY = FingerItem.y - 50; // Adjust for status bar offset

            // Check each square
            for (int i = 0; i < GRID_ROWS * GRID_COLS; i++) {
                if (checkTouch(touchX, touchY, squares[i].x, squares[i].y)) {
                    squircleFilled[i] = !squircleFilled[i]; // Toggle state
                    drawScreen();
                    delay(300); // Debounce
                    break;
                }
            }
        }
    }
}

void loop() {
    M5.update();
    handleTouch();
    delay(10);
}
