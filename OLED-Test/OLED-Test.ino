#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_WIDTH 128 // OLED display Breite, in pixels
#define SCREEN_HEIGHT 64 // OLED display Höhe, in pixels
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  Serial.begin(115200);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // I2C address = 0x3C
  delay(1000);
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print("Edi's Techlab");
  display.setCursor(0, 32);
  display.print("Edi's Techlab");


  
  display.display(); 
}

void loop() {
}
