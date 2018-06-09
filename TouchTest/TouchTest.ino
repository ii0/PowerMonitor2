
#include <Adafruit_GFX.h>     // Core graphics library
#include <SPI.h>       
#include <Adafruit_ILI9341.h>
#include <Wire.h>     
#include "XPT2046_Touchscreen.h"
#include "TouchMenu.h"

#define LED 15

#define TOUCHCS_PIN 19
XPT2046_Touchscreen ctp(TOUCHCS_PIN,255);

// The display also uses hardware SPI, plus #9 & #10
#define TFT_CS 23
#define TFT_DC 22
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

TouchMenu tm(&tft, &ctp);

Button btn(185, 280, 50, 40, "Abc");

void setup(void) {
  Serial.begin(115200);
  Serial.println(F("Cap Touch Paint!"));
  pinMode(LED,OUTPUT);
  digitalWrite(LED,HIGH);
  
  SPI.begin(14,12,13);  //  Need this to init SPI
  tft.begin();
  ctp.begin();  
  
  tft.fillScreen(ILI9341_BLACK);
  tft.setRotation(2);
  
  btn.draw();
  Serial.println("INIT DONE");
}

void loop() {
  
}

