#include <WiFi.h>
#include <WiFiManager.h>
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>
#include <SPI.h>

// ======================= PIN DEFINITIONS =======================
// ** Modify these pin numbers to match your custom PCB! **
// ---- TFT Display ----
#define TFT_CS   10
#define TFT_DC   9
#define TFT_RST  8
#define TFT_MOSI 11
#define TFT_SCLK 12
// ---- Touch Screen (XPT2046) ----
#define TOUCH_CS 14
#define TOUCH_IRQ 15
// ---- Joystick ----
#define JOY_X_PIN 1
#define JOY_Y_PIN 2
#define JOY_SW_PIN 3
// ===============================================================

// --- Objects ---
TFT_eSPI tft = TFT_eSPI();
XPT2046_Touchscreen ts(TOUCH_CS, TOUCH_IRQ);
WiFiManager wifiManager;

// --- Joystick State ---
const int JOY_CENTER_X = 2048; // Default midpoint (will calibrate)
const int JOY_CENTER_Y = 2048;
const int JOY_DEADZONE = 500;
int joyX, joyY;
bool joyButtonPressed = false;

// --- Menu State ---
enum ScreenState { IDLE_SCREEN, MAIN_MENU, WIFI_SCREEN };
ScreenState currentScreen = IDLE_SCREEN;
int menuSelection = 0;
const int MENU_ITEMS = 2; // "WiFi Setup", "Back"
const char* menuLabels[] = {"WiFi Setup", "Back"};

// --- Touch State ---
TS_Point p;
bool touched = false;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n--- HomePod Booting ---");

  // Initialize Display
  tft.init();
  tft.setRotation(1); // Adjust for your screen orientation
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(20, 50);
  tft.println("HomePod Starting...");

  // Initialize Joystick
  pinMode(JOY_SW_PIN, INPUT_PULLUP);

  // Initialize Touch
  ts.begin();
  ts.setRotation(1); // Match display rotation

  // --- WiFi Connection / Provisioning ---
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(20, 50);
  tft.println("Connecting to WiFi...");
  
  // Uncomment the following line to clear saved WiFi credentials for testing
  // wifiManager.resetSettings();

  // Fetches SSID and password and tries to connect.
  // If it fails, it starts an access point with the name "HomePod-Setup".
  bool res = wifiManager.autoConnect("HomePod-Setup");
  
  if (!res) {
    Serial.println("Failed to connect");
    tft.fillScreen(TFT_RED);
    tft.setCursor(20, 50);
    tft.println("WiFi Failed");
    delay(3000);
    ESP.restart();
  }
  
  Serial.println("WiFi connected!");
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(20, 50);
  tft.println("Connected!");
  delay(1000);

  // Go to idle screen
  showIdleScreen();
}

void loop() {
  // Read Joystick
  joyX = analogRead(JOY_X_PIN);
  joyY = analogRead(JOY_Y_PIN);
  joyButtonPressed = (digitalRead(JOY_SW_PIN) == LOW);

  // Read Touch
  touched = ts.touched();
  if (touched) {
    p = ts.getPoint();
    // Map touch coordinates to screen (adjust mapping based on calibration)
    int16_t x = map(p.x, 200, 3700, 0, tft.width());
    int16_t y = map(p.y, 240, 3800, 0, tft.height());
    handleTouch(x, y);
  }

  // State Machine
  switch (currentScreen) {
    case IDLE_SCREEN:
      handleIdleScreen();
      break;
    case MAIN_MENU:
      handleMainMenu();
      break;
    case WIFI_SCREEN:
      handleWiFiScreen();
      break;
  }

  delay(50); // Small delay for stability
}

// ======================= SCREEN HANDLERS =======================

void showIdleScreen() {
  currentScreen = IDLE_SCREEN;
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(3);
  tft.setCursor(10, 10);
  tft.println("HomePod");
  
  tft.setTextSize(2);
  tft.setCursor(10, 60);
  tft.print("IP: ");
  tft.println(WiFi.localIP());
  
  tft.setCursor(10, 100);
  tft.println("Time: --:--"); // Placeholder for NTP time
  
  tft.setCursor(10, 150);
  tft.println("Press to open menu");
}

void handleIdleScreen() {
  // Any joystick move or button press opens the menu
  if (abs(joyX - JOY_CENTER_X) > JOY_DEADZONE || 
      abs(joyY - JOY_CENTER_Y) > JOY_DEADZONE || 
      joyButtonPressed || touched) {
    
    // Simple debounce: wait for joystick to return to center
    if (abs(joyX - JOY_CENTER_X) > JOY_DEADZONE || 
        abs(joyY - JOY_CENTER_Y) > JOY_DEADZONE) {
      delay(300);
    }
    showMainMenu();
  }
}

void showMainMenu() {
  currentScreen = MAIN_MENU;
  menuSelection = 0;
  drawMainMenu();
}

void drawMainMenu() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.println("Main Menu");

  for (int i = 0; i < MENU_ITEMS; i++) {
    int yPos = 50 + i * 40;
    if (i == menuSelection) {
      tft.fillRect(5, yPos - 5, tft.width() - 10, 30, TFT_BLUE);
      tft.setTextColor(TFT_WHITE, TFT_BLUE);
    } else {
      tft.setTextColor(TFT_WHITE, TFT_BLACK);
    }
    tft.setCursor(20, yPos);
    tft.println(menuLabels[i]);
  }
}

void handleMainMenu() {
  // Navigate with joystick Y-axis
  if (joyY < JOY_CENTER_Y - JOY_DEADZONE) { // Up
    menuSelection--;
    if (menuSelection < 0) menuSelection = MENU_ITEMS - 1;
    drawMainMenu();
    delay(300); // Debounce
  } else if (joyY > JOY_CENTER_Y + JOY_DEADZONE) { // Down
    menuSelection++;
    if (menuSelection >= MENU_ITEMS) menuSelection = 0;
    drawMainMenu();
    delay(300);
  }

  // Select with joystick button or touch
  if (joyButtonPressed) {
    delay(200); // Debounce
    if (menuSelection == 0) {
      showWiFiScreen();
    } else if (menuSelection == 1) {
      showIdleScreen();
    }
  }

  // Touch selection (simple mapping: touch the menu item)
  if (touched) {
    if (p.y > 1000 && p.y < 2000) { // Rough zone for first item
      if (menuSelection == 0) showWiFiScreen();
    } else if (p.y > 2000 && p.y < 3000) { // Rough zone for second item
      if (menuSelection == 1) showIdleScreen();
    }
  }
}

void showWiFiScreen() {
  currentScreen = WIFI_SCREEN;
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.println("WiFi Setup");
  tft.setTextSize(1);
  tft.setCursor(10, 50);
  tft.println("Connect to 'HomePod-Setup'");
  tft.setCursor(10, 70);
  tft.println("and go to 192.168.4.1");
  tft.setCursor(10, 110);
  tft.println("Press joystick to return");
}

void handleWiFiScreen() {
  if (joyButtonPressed || touched) {
    delay(300);
    showMainMenu();
  }
}

// ======================= TOUCH HANDLER =======================

void handleTouch(int16_t x, int16_t y) {
  // Basic touch handling; can be expanded for gestures
  Serial.print("Touch: "); Serial.print(x); Serial.print(", "); Serial.println(y);
  
  // Example: tap anywhere in the idle screen to open menu
  if (currentScreen == IDLE_SCREEN) {
    showMainMenu();
  }
  // In the menu, tapping is handled in handleMainMenu() based on Y position
}
