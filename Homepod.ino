#include <SPI.h>
#include <TFT_eSPI.h>
#include <WiFi.h>
#include <WiFiManager.h>

// Pins for the 7Semi ESP32-S3 layout
const int PIN_JOY_X  = 4;
const int PIN_JOY_Y  = 5;
const int PIN_JOY_SW = 6;

TFT_eSPI screen = TFT_eSPI();

// UI view navigation states
#define VIEW_EYES       0
#define VIEW_MENU       1
#define VIEW_CHAT       2
#define VIEW_WIFI       3
int currentView = VIEW_EYES;

// Menu setup parameters
int menuSelection = 0;
const int maxMenuItems = 3;
const char* menuList[] = {"1. Start AI Chat", "2. Change AI Model", "3. Set Up Wi-Fi"};

// Groq API model array list
const char* aiModels[] = {"llama3-8b-8192", "llama3-70b-8192", "gemma2-9b-it"};
int activeModelIndex = 0;

// Default dashboard display values
String currentTime = "16:40";
String currentEstWeather = "Bengaluru: 24 C - Clear";

// Variables to handle timing and eye movement animations
unsigned long nextEyeMoveTime = 0;
int xEyeOffset = 0;
int yEyeOffset = 0;

// Debounce timer logic variables
unsigned long inputDebounceTimer = 0;
const int inputInterval = 250;

void setup() {
  Serial.begin(115200);
  pinMode(PIN_JOY_SW, INPUT_PULLUP);

  screen.init();
  screen.setRotation(1); // Set to landscape mode orientation layout
  screen.fillScreen(TFT_BLACK);

  screen.setTextColor(TFT_WHITE);
  screen.setTextSize(2);
  screen.drawString("Connecting to Wi-Fi...", 10, 20);

  // Starts an on-device Wi-Fi manager configuration setup portal
  WiFiManager wifiManager;
  if (!wifiManager.autoConnect("DeskCompanion_AP")) {
    screen.fillScreen(TFT_RED);
    screen.drawString("Connection Failed.", 10, 60);
    delay(3000);
    ESP.restart();
  }

  screen.fillScreen(TFT_BLACK);
}

void loop() {
  // Read inputs from screen touch panel and joystick variables
  int xVal = analogRead(PIN_JOY_X);
  int yVal = analogRead(PIN_JOY_Y);
  bool stickPressed = (digitalRead(PIN_JOY_SW) == LOW);
  
  uint16_t touchX = 0, touchY = 0;
  bool screenTouched = screen.getTouch(&touchX, &touchY);

  // Navigation state processing checks
  if (millis() - inputDebounceTimer > inputInterval) {
    
    if (currentView == VIEW_EYES) {
      // Open the system menu if joystick is pushed up or screen panel is touched
      if (yVal < 1000 || screenTouched) {
        currentView = VIEW_MENU;
        menuSelection = 0;
        screen.fillScreen(TFT_BLACK);
        inputDebounceTimer = millis();
      }
    } 
    else if (currentView == VIEW_MENU) {
      // Push down to move the selection pointer forward
      if (yVal > 3000) {
        menuSelection = (menuSelection + 1) % maxMenuItems;
        inputDebounceTimer = millis();
      } 
      // Push up to move the selection pointer backward
      else if (yVal < 1000) {
        menuSelection = (menuSelection - 1 + maxMenuItems) % maxMenuItems;
        inputDebounceTimer = millis();
      }
      
      // Select the active highlighted item on button click
      if (stickPressed) {
        if (menuSelection == 0) currentView = VIEW_CHAT;
        if (menuSelection == 1) activeModelIndex = (activeModelIndex + 1) % 3;
        if (menuSelection == 2) currentView = VIEW_WIFI;
        
        screen.fillScreen(TFT_BLACK);
        inputDebounceTimer = millis();
      }
    } 
    else if (currentView == VIEW_CHAT || currentView == VIEW_WIFI) {
      // Exit active modes and drop back down into idle eyes layout view on button click
      if (stickPressed) {
        currentView = VIEW_EYES;
        screen.fillScreen(TFT_BLACK);
        inputDebounceTimer = millis();
      }
    }
  }

  // Draw the current visual content depending on active screen view
  if (currentView == VIEW_EYES) {
    // Bold clock readout metrics lines
    screen.setTextColor(TFT_GREEN, TFT_BLACK);
    screen.setTextSize(4);
    screen.drawString(currentTime, 110, 10);

    // Weather readout strings lines
    screen.setTextSize(2);
    screen.setTextColor(TFT_WHITE, TFT_BLACK);
    screen.drawString(currentEstWeather, 10, 60);

    // Dynamic eyes graphics updates
    if (millis() > nextEyeMoveTime) {
      xEyeOffset = random(-12, 13);
      yEyeOffset = random(-6, 7);
      nextEyeMoveTime = millis() + random(1500, 3000);
      screen.fillRect(40, 95, 240, 120, TFT_BLACK);
    }

    // Left outer eyeball outline
    screen.fillEllipse(80, 155, 35, 45, TFT_WHITE);
    screen.fillCircle(80 + xEyeOffset, 155 + yEyeOffset, 14, TFT_BLUE);
    screen.fillCircle(80 + xEyeOffset, 155 + yEyeOffset, 6, TFT_BLACK);

    // Right outer eyeball outline
    screen.fillEllipse(240, 155, 35, 45, TFT_WHITE);
    screen.fillCircle(240 + xEyeOffset, 155 + yEyeOffset, 14, TFT_BLUE);
    screen.fillCircle(240 + xEyeOffset, 155 + yEyeOffset, 6, TFT_BLACK);
  } 
  else if (currentView == VIEW_MENU) {
    screen.setTextColor(TFT_CYAN, TFT_BLACK);
    screen.setTextSize(3);
    screen.drawString("SYSTEM MENU", 60, 15);

    screen.setTextSize(2);
    for (int i = 0; i < maxMenuItems; i++) {
      if (i == menuSelection) {
        screen.setTextColor(TFT_YELLOW, TFT_BLUE); // Invert colors for highlighted cursor selection
      } else {
        screen.setTextColor(TFT_WHITE, TFT_BLACK);
      }
      screen.drawString(menuList[i], 30, 80 + (i * 35));
    }

    screen.setTextColor(TFT_DARKGREEN, TFT_BLACK);
    screen.drawString("Model: " + String(aiModels[activeModelIndex]), 30, 205);
  } 
  else if (currentView == VIEW_CHAT) {
    screen.setTextColor(TFT_MAGENTA, TFT_BLACK);
    screen.drawString("Groq AI Terminal Mode", 10, 15);
    screen.setTextColor(TFT_WHITE, TFT_BLACK);
    screen.drawString("Waiting for Serial strings...", 10, 60);
    screen.drawString("Using Model: " + String(aiModels[activeModelIndex]), 10, 100);
    screen.drawString("Click stick to exit", 10, 200);
  } 
  else if (currentView == VIEW_WIFI) {
    screen.setTextColor(TFT_RED, TFT_BLACK);
    screen.drawString("Wi-Fi Setup Access Point Active", 10, 20);
    screen.setTextColor(TFT_WHITE, TFT_BLACK);
    screen.drawString("Connect phone to configuration AP:", 10, 60);
    screen.drawString("SSID: DeskCompanion_AP", 10, 100);
    screen.drawString("Click stick to close view", 10, 200);

    WiFiManager wm;
    wm.startConfigPortal("DeskCompanion_AP");
    
    // Once configuration routine is closed or submitted, return straight back to main dashboard view
    currentView = VIEW_EYES;
    screen.fillScreen(TFT_BLACK);
  }

  delay(20);
}
