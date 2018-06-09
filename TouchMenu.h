#ifndef _TOUCHMENUH_
#define _TOUCHMENUH_

#include "XPT2046_Touchscreen.h"

#define TOUCHCS_PIN 19
#define TOUCH_IRQ 21

#define maxButtons 10

// This is calibration data for the raw touch data to the screen coordinates
#define TS_MINX 650
#define TS_MINY 550
#define TS_MAXX 3500
#define TS_MAXY 3500


XPT2046_Touchscreen ts(TOUCHCS_PIN,255);

struct menuButton {
  int x;
  int y;
  int width;
  int height;
  String text;
  uint16_t bcolor;
  uint16_t tcolor;  
};

struct menuSelect {
  String text;
  uint16_t bcolor;
  uint16_t tcolor;  
  boolean selected;
};

class MyTouchMenu {
  private:
    menuButton buttons[maxButtons];
    int totalButtons;
    boolean changed;
    int highlightedButton, lastHighlightedButton;
    void showButton(menuButton button, boolean highlight);
  
  public:
    TS_Point lastPoint;
    boolean newButton;
    int buttonTextSize;
    
  	MyTouchMenu();
    
    boolean touched();
    void addButton(int x, int y, int width, int height, String text, uint16_t bcolor, uint16_t tcolor);
  	void showButtons(boolean forced = false);
    int checkButtonPressed();
    void pressCheck();
    boolean centerTouch();
};

class MyTouchMenuSelection {
  private:
    menuSelect selections[maxButtons];
    int totalSelections;
    int buttonSpacing;
    int width;
    int height;
    TS_Point p;
    boolean changed;

    void showSelection(int i, menuSelect button);

  public:
    int buttonTextSize;
    uint16_t borderColor;

    MyTouchMenuSelection();

    void setPosition(int x, int y);
    void addSelection(String text, uint16_t bcolor, uint16_t tcolor);
    void showSelections(boolean forced = false);
    int checkButtonPressed();
};


MyTouchMenu touchMenu;



MyTouchMenuSelection::MyTouchMenuSelection() {
  totalSelections = 0;  
  buttonTextSize = 1;
  width = 0;
  height = 0;
  buttonSpacing = 50;
  borderColor = ILI9341_BLUE;
  setPosition(0,0);
}

void MyTouchMenuSelection::setPosition(int x, int y) {
  p.x = x;
  p.y = y; 
}

void MyTouchMenuSelection::addSelection(String text, uint16_t bcolor, uint16_t tcolor) {
  if(totalSelections<maxButtons) {
    height = height + (buttonTextSize*buttonSpacing);
    int sWidth = (buttonTextSize*6 * text.length()) + 50;
    if(sWidth>width) width = sWidth;

    Serial.println(text+"  "+String(p.x)+","+String(p.y)+","+String(width)+","+String(height));
    
    selections[totalSelections].text = text;
    selections[totalSelections].bcolor = bcolor;
    selections[totalSelections].tcolor = tcolor;
    selections[totalSelections].selected = false;
    if(totalSelections == 1) selections[totalSelections].selected = true;
    totalSelections++;  
  }
}

void MyTouchMenuSelection::showSelection(int i, menuSelect button) {
  tft.setTextColor(button.tcolor);
  tft.setTextSize(buttonTextSize);
  if(button.selected) tft.fillRect(p.x+10,p.y+(i*buttonTextSize*buttonSpacing)+17,width-20,(buttonTextSize*20), button.bcolor);
  else tft.fillRoundRect(p.x+10,p.y+(i*buttonTextSize*buttonSpacing)+17,width-20,(buttonTextSize*20), 8, button.bcolor);
  tft.setCursor(p.x+(width/2)-(buttonTextSize*6*(button.text.length()/2)), p.y+(i*buttonTextSize*buttonSpacing)+23);
  tft.print(button.text);  
  Serial.println(button.text+"  "+String(tft.getCursorX())+","+String(tft.getCursorY()));
}

void MyTouchMenuSelection::showSelections(boolean forced) {
  if(changed || forced) {
    for(int i=0;i<totalSelections;i++) { 
      showSelection(i,selections[i]);  
    }
    tft.drawRoundRect(p.x,p.y,width,height+10, 8, borderColor);
    tft.drawRoundRect(p.x+1,p.y+1,width-2,height+10-2, 8, borderColor);
  }
  changed = false;
}

int MyTouchMenuSelection::checkButtonPressed() {
  TS_Point p = ts.getPoint();
  if(ts.touched()) changed = true;
  
  p.x = map(p.x, TS_MINX, TS_MAXX, tft.width(), 0);
  p.y = map(p.y, TS_MINY, TS_MAXY, tft.height(), 0);

  int pButton = -1;
  if(ts.touched()) {
    for(int i=0;i<totalSelections;i++) { 
      int x = p.x+10;
      int y = p.y+(i*buttonTextSize*buttonSpacing)+17;
      int wid = width-20;
      int hght = (buttonTextSize*20);
      //if((p.x>=selections[i].x) && (p.y>=selections[i].y) && (p.x<=selections[i].x+selections[i].width) && (p.y<=selections[i].y+selections[i].height))
        pButton = i;
    }
  }
  //lastHighlightedButton = highlightedButton;
  //highlightedButton = pButton;
  //if(highlightedButton != lastHighlightedButton) changed = true;
  showSelections();

  return pButton;
}





void read_touch_ISR() 
{
  touchMenu.newButton = true;
}

MyTouchMenu::MyTouchMenu() {
  newButton = false;
  totalButtons = 0;
  highlightedButton = -1;
  buttonTextSize = 1;
  changed = true;
  pinMode(TOUCHCS_PIN,OUTPUT);
  digitalWrite(TOUCHCS_PIN,HIGH);	
  pinMode(TOUCH_IRQ,INPUT_PULLUP);
  attachInterrupt(TOUCH_IRQ,read_touch_ISR,RISING);
}

void MyTouchMenu::addButton(int x, int y, int width, int height, String text, uint16_t bcolor, uint16_t tcolor) {
  if(totalButtons<maxButtons) {
    buttons[totalButtons].x = x;
    buttons[totalButtons].y = y;
    buttons[totalButtons].width = width;
    buttons[totalButtons].height = height;
    buttons[totalButtons].text = text;
    buttons[totalButtons].bcolor = bcolor;
    buttons[totalButtons].tcolor = tcolor;
    totalButtons++;  
  }
}

void MyTouchMenu::showButton(menuButton button, boolean highlight) {
  //if(highlight) fcolor = ILI9341_WHITE;
  tft.fillRoundRect(button.x, button.y, button.width, button.height, 8, button.bcolor);
  tft.drawRoundRect(button.x, button.y,button. width, button.height, 8, ILI9341_WHITE);
  if(highlight) tft.drawRoundRect(button.x+4, button.y+4,button. width-8, button.height-8, 8, ILI9341_WHITE);
  tft.setTextColor(button.tcolor);
  tft.setTextSize(buttonTextSize);
  tft.setCursor(button.x+(button.width/2)-(buttonTextSize*6*(button.text.length()/2)), button.y+(button.height/2)-(buttonTextSize*4));
  tft.print(button.text);  
}

void MyTouchMenu::showButtons(boolean forced) {
  if(changed || forced) {
    for(int i=0;i<totalButtons;i++) 
      showButton(buttons[i],(i==highlightedButton));  
  }
  changed = false;
}

boolean MyTouchMenu::centerTouch() {
  TS_Point p = ts.getPoint();
  
  p.x = map(p.x, TS_MINX, TS_MAXX, tft.width(), 0);
  p.y = map(p.y, TS_MINY, TS_MAXY, tft.height(), 0);

  if(ts.touched()) {
    if((p.x>=((tft.width()/2)-50)) && (p.y>=((tft.height()/2)-50)) && (p.x<=((tft.width()/2)+50)) && (p.y<=((tft.height()/2)+50))) return true;
    else return false; 
  } else return false;   
}

int MyTouchMenu::checkButtonPressed() {
  TS_Point p = ts.getPoint();
  if(ts.touched()) changed = true;
  
  p.x = map(p.x, TS_MINX, TS_MAXX, tft.width(), 0);
  p.y = map(p.y, TS_MINY, TS_MAXY, tft.height(), 0);

  int pButton = -1;
  if(ts.touched()) {
    for(int i=0;i<totalButtons;i++) { 
      if((p.x>=buttons[i].x) && (p.y>=buttons[i].y) && (p.x<=buttons[i].x+buttons[i].width) && (p.y<=buttons[i].y+buttons[i].height))
        pButton = i;
    }
  }
  lastHighlightedButton = highlightedButton;
  highlightedButton = pButton;
  if(highlightedButton != lastHighlightedButton) changed = true;
  showButtons();

  return pButton;
}

void MyTouchMenu::pressCheck() {
  tft.fillScreen(ILI9341_BLACK);
  menuButton button;
  
  TS_Point p = ts.getPoint();
  Serial.println("Raw: " + String(p.x) + "," + String(p.y));
  p.x = map(p.x, TS_MINX, TS_MAXX, tft.width(), 0);
  p.y = map(p.y, TS_MINY, TS_MAXY, tft.height(), 0);
  Serial.println("Scaled: " + String(p.x) + "," + String(p.y));
  Serial.println("");
  
  button.x = p.x-20;
  button.y = p.y-20;
  button.width = 40;
  button.height = 40;
  button.bcolor = ILI9341_YELLOW;
  button.tcolor = ILI9341_BLACK;
  button.text = "*";    
  showButton(button,false);
  delay(250);
}

boolean MyTouchMenu::touched() {
  return ts.touched();  
}

#endif
