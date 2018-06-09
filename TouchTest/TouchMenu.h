#ifndef TouchMenu_h
#define TouchMenu_h

#include <Arduino.h>
#include "Adafruit_ILI9341.h"
#include "XPT2046_Touchscreen.h"

#define BTN_BEEP_DELAY1 150
#define BTN_BEEP_DELAY2 15

#define BTN_RADIUS 10

#define COL_BTN_BG       0x000A
#define COL_BTN_BORDER   ILI9341_WHITE
#define COL_BTN_PRESSED  ILI9341_DARKGREY

class TouchMenu
{
  public:
    TouchMenu(Adafruit_ILI9341 *tft, XPT2046_Touchscreen *ctp);
    void beep(uint16_t beepDelay);
 
    Adafruit_ILI9341 *_tft;
    XPT2046_Touchscreen *_ctp;
};


class Button : public TouchMenu {
  public:
    Button(int16_t x, int16_t y, int16_t w, int16_t h, char *text);
    void draw();
  private:
    int16_t _x;
    int16_t _y;
    int16_t _w;
    int16_t _h;
    char *_text;
};


 
#endif
