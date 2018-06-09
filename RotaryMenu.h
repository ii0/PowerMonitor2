#ifndef _ROTARYMENUH_
#define _ROTARYMENUH_

#include "Scope.h"
//#include "XPT2046_Touchscreen.h"

//#define TOUCHCS_PIN 19
//#define TOUCH_IRQ 21
#define QUADA 26
#define QUADB 27
#define SEL 18


#define RIGHTMARGIN         70
#define LEFTMARGIN          20
#define BOTTOMMARGIN        60
#define TOPMARGIN           17
#define UPPERPLOTMARGIN     TOPMARGIN + 10
#define MENUITEMWIDTH       100                 // The pixel width of the background of main menu item
#define INTERMENUSPACING    13


#define ELEMENTS(x) (sizeof(x) / sizeof(x[0]))


//XPT2046_Touchscreen ts(TOUCHCS_PIN,255);
double bandEdges[] = {1800, 2000, 3500, 4000, 5330, 5403, 7000, 7300, 10100, 10150, 14000, 14350, 18068, 18158, 21000, 21450, 24890, 24990, 28000, 29700, 10000, 20000};

const char *menuLevel1[]  = {" Band ", " Options ", " Traces ", " Scan "};
const char *menuBands[]   = {"All", "160M (1.800-2.000MHz)", "80M (3.500-4.000MHz)", "60M (5.330-5.403MHz)", "40M (7.000-7.300MHz)", "30M (10.100-10.150MHz)", 
  "20M (14.000-14.350MHz)", "17M (18.068-18.158MHz)", "15M (21.000-21.450MHz)", "12M (24.890-24.990MHz)", "10M (28.000-29700MHz)", "Test Load (10.000-20,000MHz)"};
const char *menuFile[]    = {"Save Scan", "View Plot", "View Table", "Overlay", "Serial", "Delete File"};
const char *menuTraces[]  = {"VSWR", "VSWR/Z", "SWR/Z/X/R", "Vf/Vr/Vz/Va"};
const char *menuBlank[]   = {""};

// Colors
int BLUE    = 0x001F;
int GREEN   = 0x07E0;
int RED     = 0xF800;
int YELLOW  = 0xFFE0;
int WHITE   = 0xFFFF;
int BLACK   = 0x0000;
int DKGREEN = 0x03E0;
int LTPINK  = 0xFDDF;
int LTGREY  = ~0xE71C;
int MAGENTA = 0xF81F;

class MyRotaryMenu {
  private:
    int row, col;
    int rotaryTicsPerChange = 4;
    boolean CursorMoveEnable;
    
  public:
    int menuIndex, lastMenuIndex;
    int subMenuIndex[8]; 
    int lastSubMenuIndex;
    int currentMenuElements;
    boolean inSubMenu;
    boolean menuChange;
    boolean subMenuChange;
    
	  MyRotaryMenu();
    void ShowMenu(const char *whichMenu[], int len, boolean force = false); 
    void ShowSubMenu(const char *menu[], int len);
    void SetmenuIndex();
    void CheckSEL();
    void Splash();
};

MyRotaryMenu rotaryMenu;

////////////////////////////////
//  Rotary Encoder Interrupt  //
////////////////////////////////
int rotaryCount = 0;
boolean rotaryChange = false;
void read_encoder_ISR() 
{
  static uint8_t old_AB = 0;
  // grey code
  // http://hades.mech.northwestern.edu/index.php/Rotary_Encoder
  // also read up on 'Understanding Quadrature Encoded Signals'
  // https://www.pjrc.com/teensy/td_libs_Encoder.html
  // another interesting lib: https://github.com/0xPIT/encoder/blob/arduino/ClickEncoder.cpp
  static int8_t enc_states[] = {0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0};

  old_AB <<= 2;
  old_AB |= ((digitalRead(QUADA))?(1<<1):0) | ((digitalRead(QUADB))?(1<<0):0);
  rotaryCount += ( enc_states[( old_AB & 0x0f )]);
  rotaryChange = true;
}


MyRotaryMenu::MyRotaryMenu() {
  pinMode(QUADA,INPUT_PULLUP);
  pinMode(QUADB,INPUT_PULLUP);
  pinMode(SEL,INPUT_PULLUP);
  attachInterrupt(QUADA,read_encoder_ISR,CHANGE);
  attachInterrupt(QUADB,read_encoder_ISR,CHANGE);

  rotaryCount = 4 * 3;
  menuIndex = 3; lastMenuIndex = 0;
  inSubMenu = false;
  menuChange = true;
  CursorMoveEnable = false;
  subMenuChange = true;
  subMenuIndex[0] = 0;
  subMenuIndex[1] = 0;
  subMenuIndex[2] = 3;
  subMenuIndex[3] = 0;
}

void MyRotaryMenu::CheckSEL() {
  while(digitalRead(SEL) == LOW) {
    long Hold = millis();
    if(menuIndex==3) {
      CursorMoveEnable = !CursorMoveEnable;  
    } else inSubMenu = !inSubMenu;
    while(millis() < (Hold+100)) {
      if(digitalRead(SEL) == LOW) Hold = millis();  
    }
    rotaryChange = true;
    if(!inSubMenu) rotaryCount = 3 * rotaryTicsPerChange;
    SetmenuIndex();
    ShowMenu(menuLevel1, ELEMENTS(menuLevel1),true);
  }  
}

void MyRotaryMenu::SetmenuIndex() {
  if(!CursorMoveEnable) {
    if(rotaryCount < 0) rotaryCount = 0;
    if(rotaryCount > (rotaryTicsPerChange * (currentMenuElements-1))) rotaryCount = (rotaryTicsPerChange * (currentMenuElements-1));
    
    if(inSubMenu) subMenuIndex[menuIndex] = rotaryCount / rotaryTicsPerChange;
    else menuIndex = rotaryCount / rotaryTicsPerChange;
    
    if(menuIndex != lastMenuIndex) menuChange = true;
    if(menuIndex != lastMenuIndex) subMenuChange = true;
    if(subMenuIndex[lastMenuIndex] != lastSubMenuIndex) subMenuChange = true;
    
    lastMenuIndex    = menuIndex;
    lastSubMenuIndex = subMenuIndex[menuIndex];
  } else {
    if(rotaryCount/4 > scope.traceWidth) rotaryCount = scope.traceWidth * 4;
    scope.XCursor = rotaryCount/4;  
  }
  rotaryChange = false;
}

void MyRotaryMenu::Splash() {
  int row, col;
  tft.fillScreen(BLACK);

  row = tft.height() / 5;
  col = tft.width() / 4;
  tft.setTextSize(2);

  tft.setTextColor(MAGENTA, BLACK);
  tft.setCursor(6, row - INTERMENUSPACING * 2);
  tft.print(F("Milford Amateur Radio Club"));
  
  tft.setTextColor(RED, BLACK);
  tft.setCursor(col, row);
  tft.print(F("Antenna Analyzer"));
  tft.setTextSize(1);
  tft.setTextColor(WHITE, BLACK);
  col = tft.width() / 2;
  tft.setCursor(col, row + INTERMENUSPACING * 2);
  tft.print("by");
  col = tft.width() / 3;
  tft.setTextSize(2);
  tft.setTextColor(GREEN, BLACK);
  tft.setCursor(col - 20, row + INTERMENUSPACING * 4);
  tft.print(F("Eric Statler, WWWWW"));

  delay(3000);
  tft.fillScreen(BLACK);
}

void MyRotaryMenu::ShowMenu(const char *whichMenu[], int len, boolean force) {
  CheckSEL();
  int currentMenuIndex = menuIndex;

  if((menuChange) || force) {
    if(~inSubMenu) currentMenuElements = len;
    int spacing = (tft.width() - 20) / len;       
    tft.setTextColor(WHITE, BLACK);
    tft.setTextSize(1);
    for (int i = 0; i < len; i++) {
      tft.setCursor(LEFTMARGIN+(i * spacing), 0);
      tft.print(whichMenu[i]);
    }
    tft.setCursor(LEFTMARGIN+(currentMenuIndex * spacing), 0);
    if(inSubMenu) tft.setTextColor(BLUE, LTGREY);
    else tft.setTextColor(BLUE, WHITE);
    tft.print(whichMenu[currentMenuIndex]);
    row = 0;
    col = LEFTMARGIN+(currentMenuIndex * spacing);
    menuChange = false;
  }
  
  if((subMenuChange) || (menuChange) || force) {
    switch(currentMenuIndex) {
      case 0:  ShowSubMenu(menuBands,ELEMENTS(menuBands));   break;
      case 1:  ShowSubMenu(menuFile,ELEMENTS(menuFile));     break;
      case 2:  ShowSubMenu(menuTraces,ELEMENTS(menuTraces)); break;
      default: ShowSubMenu(menuBlank,ELEMENTS(menuBlank));   break;
    } 
  } 
}

void MyRotaryMenu::ShowSubMenu(const char *menu[], int len) {
  if(inSubMenu) currentMenuElements = len;
  
  int w = tft.width();
  int h = tft.height();
  if(!inSubMenu) tft.fillRect(0, TOPMARGIN-1, w + 10, h, BLACK);    // Erase screen below top menu
  tft.setTextColor(GREEN, BLACK);
  tft.setTextSize(1);
  for (int i = i; i < len; i++) {
    tft.setCursor(col, i * INTERMENUSPACING + TOPMARGIN);
    tft.print(menu[i]);
  }
  tft.setCursor(col, subMenuIndex[menuIndex] * INTERMENUSPACING + TOPMARGIN);
  if(inSubMenu) tft.setTextColor(BLUE, WHITE);
  else tft.setTextColor(BLUE, LTGREY);
  if(subMenuIndex[menuIndex]<len) tft.print(menu[subMenuIndex[menuIndex]]);
  subMenuChange = false;
}

#endif
