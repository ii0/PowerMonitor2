#include "Arduino.h"
#include "TouchMenu.h"

TouchMenu::TouchMenu(Adafruit_ILI9341 *tft, XPT2046_Touchscreen *ctp)
{
 _tft = tft;
 _ctp = ctp;
}

Button::Button(int16_t x, int16_t y, int16_t w, int16_t h, char *text) : TouchMenu(_tft,_ctp)
{
  _x = x;
  _y = y;
  _w = w;
  _h = h;
  _text = text;
}

void Button::draw()
{
  _tft->fillRoundRect(_x, _y, _w, _h, BTN_RADIUS, COL_BTN_BG);
  _tft->drawRoundRect(_x, _y, _w, _h, BTN_RADIUS, COL_BTN_BORDER);
  _tft->setCursor(_x+_w/2, _y+_h/2);
  _tft->setTextSize(2);
  _tft->print(_text);
}
