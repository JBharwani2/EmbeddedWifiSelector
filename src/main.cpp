#include <M5Core2.h>
#include "WiFi.h"
#include <String.h>
#include <list>

int n;
int ssidLength = 9;
int thisPage = 0;
int selectedNetwork = 0;
const int pageSize = 10;
bool wifiPage = false;
bool leftLocked = false;
bool rightLocked = false;
String wifiName = "";
String wifiPassword = "";
std::list<String> networkList;

TFT_eSprite window = TFT_eSprite(&M5.Lcd);

Button sc(150, 125, 65, 35);

void DrawMenu();
void LCD_Clear();
void DrawMenu();
void Show(int nav);
void Search();
void Select();
void IncrementNum();
void DecrementNum();
void ButtonAction(Event& e);


void setup()
{
    M5.begin();

    // Handle button presses
    M5.Buttons.addHandler(ButtonAction,  E_TOUCH);

    // Draw Start Page
    window.setColorDepth(8);
    window.createSprite(250, 160);
    window.fillSprite(CYAN);
    window.setTextSize(2);
    window.setTextColor(BLACK);
    window.drawString("Wi-Fi scanner", 55, 25);
    DrawMenu();
}

void loop()
{
  if (M5.BtnA.wasPressed()) DecrementNum();
  if (M5.BtnB.wasPressed()) Select();
  if (M5.BtnC.wasPressed()) IncrementNum();
  M5.update();
}

void DrawMenu(){
    // Always draw the scan button
    window.fillRect(90, 110, 65, 35, BLACK); 
    window.setTextColor(BLUE);
    window.drawString("SCAN", 100, 120);
    window.pushSprite(60, 25);
}

void LCD_Clear() {
    // Reset the screen between changes
    window.fillSprite(CYAN);
    window.setTextColor(BLACK);
    window.setTextSize(2);
    window.pushSprite(60, 25);
}

void Show(int nav = 0){
    LCD_Clear();

    // Print total networks found at top of screen
    window.setCursor(75, 2);
    window.printf("TOTAL: %d", n);
    window.setCursor(0, 30);

    // Print list of networks
    for (int i = (thisPage * pageSize); i < ((thisPage * pageSize) + pageSize); i++)
    {
        if (i >= n) break;

        // Selecting a network
        if(i == selectedNetwork) {
            window.setTextColor(BLUE);
            wifiName = WiFi.SSID(i);
        } else {
            window.setTextColor(BLACK);
        }
        networkList.push_back(WiFi.SSID(i));

        
        window.print(i + 1);
        String ssid = (WiFi.SSID(i).length() > ssidLength)
        ? (WiFi.SSID(i).substring(0, ssidLength) + "...") 
        : WiFi.SSID(i);
        
        String strength;
        if(WiFi.RSSI(i) < -90) {
            strength = "*";
        } else if(WiFi.RSSI(i) < -75) {
            strength = "**";
        } else {
            strength = "***";
        }
        
        window.print(") " + ssid + " " + strength + "\n");
    }
    // Draw arrows and select buttons
    wifiPage = true;
    window.setTextColor(BLUE);
    window.fillRect(60, 110, 35, 35, BLACK); 
    window.drawString("<", 70, 120);
    window.fillRect(150, 110, 35, 35, BLACK); 
    window.drawString(">", 160, 120);
    // window.fillRect(205, 110, 35, 35, BLACK); 
    // window.drawString("O", 220, 120);
    DrawMenu();
}

void Search() {
    // Initialize WiFi in reciever mode
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    // Loading Screen
    LCD_Clear();
    window.drawString("Please wait.", 55, 50);
    window.drawString("Searching...", 55, 70);
    window.pushSprite(60, 25);
    Serial.println("Searching networks.....");
    
    // Get info from scanned networks
    n = WiFi.scanNetworks();
    Show();
}

void Select() {
    LCD_Clear();
    int waitCount = 0;
    window.setTextSize(3);
    Serial.println(wifiName);

    // Connect to WiFi
    WiFi.begin(wifiName.c_str(), wifiPassword.c_str());
    Serial.printf("Connecting");
    while (WiFi.status() != WL_CONNECTED && waitCount < 10) {
        delay(500);
        Serial.print(".");
        waitCount++;
    }

    // Draw results
    if(WiFi.status() == WL_CONNECTED) {
        Serial.print("\n\nConnected to WiFi network with IP address: ");
        Serial.println(WiFi.localIP());
        window.setTextColor(BLUE);
        window.setCursor(50, 30);
        window.printf("CONNECTION");
        window.setCursor(50, 60);
        window.printf("SUCCESSFUL");
        window.setCursor(50, 90);
        window.printf(wifiName.c_str());
        window.pushSprite(60, 25);
        M5.Spk.DingDong();
        M5.Axp.SetLDOEnable(3, true);
        delay(1000);
        M5.Axp.SetLDOEnable(3, false);
    } else {
        window.setTextColor(RED);
        window.setCursor(50, 60);
        window.printf("CONNECTION");
        window.setCursor(50, 90);
        window.printf("FAILED");
        window.pushSprite(60, 25);
    }
    
}

// Moves up and down the list of
void IncrementNum()
{
    selectedNetwork = selectedNetwork + 1;
    if (selectedNetwork >= n)
    {
        selectedNetwork = selectedNetwork % n;
    }
    Show();
}

void DecrementNum()
{
    selectedNetwork = selectedNetwork - 1;
    if (selectedNetwork < 0)
    {
        selectedNetwork = n - 1;
    }
    Show();
}

// Button 4 triggers search
void ButtonAction(Event& e) {
    Button& b = *e.button;
    if(b.instanceIndex() == 4) {
        Search();
    }
}
