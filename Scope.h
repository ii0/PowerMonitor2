#ifndef _SCOPEH_
#define _SCOPEH_


#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"


#define TFT_DC 22
#define TFT_CS 23
#define TFT_MOSI 13
#define TFT_MISO 12
#define TFT_SCLK 14
#define TFT_LED 15
#define TFT_RESET 1

#define ILI9341_BLACK       0x0000      /*   0,   0,   0 */
#define ILI9341_NAVY        0x000F      /*   0,   0, 128 */
#define ILI9341_DARKGREEN   0x03E0      /*   0, 128,   0 */
#define ILI9341_DARKCYAN    0x03EF      /*   0, 128, 128 */
#define ILI9341_MAROON      0x7800      /* 128,   0,   0 */
#define ILI9341_PURPLE      0x780F      /* 128,   0, 128 */
#define ILI9341_OLIVE       0x7BE0      /* 128, 128,   0 */
#define ILI9341_LIGHTGREY   0xC618      /* 192, 192, 192 */
#define ILI9341_DARKGREY    0x7BEF      /* 128, 128, 128 */
#define ILI9341_BLUE        0x001F      /*   0,   0, 255 */
#define ILI9341_GREEN       0x07E0      /*   0, 255,   0 */
#define ILI9341_CYAN        0x07FF      /*   0, 255, 255 */
#define ILI9341_RED         0xF800      /* 255,   0,   0 */
#define ILI9341_MAGENTA     0xF81F      /* 255,   0, 255 */
#define ILI9341_YELLOW      0xFFE0      /* 255, 255,   0 */
#define ILI9341_WHITE       0xFFFF      /* 255, 255, 255 */
#define ILI9341_ORANGE      0xFD20      /* 255, 165,   0 */
#define ILI9341_GREENYELLOW 0xAFE5      /* 173, 255,  47 */
#define ILI9341_PINK        0xF81F
#define CURSORCOLOR         ILI9341_OLIVE

///////////////////////
//  320(w) x 240(h)  //
///////////////////////
class MyScope {
  private:
    int erase[4][400];
    uint16_t TraceColor(int Trace);
    void ShowAxis();
    void DrawGrid();
    void EraseTrace(int Trace, int Offset);
    void DrawTrace(int Trace, int Offset, uint16_t Color);
    double pointToFreq(int p);
    
  public:
    long Count;
    int data[4][510];
    double cursorValues[4];
    String TraceNames[4];
    int width, height;
    int leftmargin, rightmargin;
    int topmargin, bottommargin;
    int xdiv, ydiv;
    int traceWidth;
    int XCursor, lastXCursor;
    double startMHz;
    double endMHz;
    double LeftYMax;
    double RightYMax;
    double RightYScale;
    double LeftYScale;

    MyScope();

    void begin();
    void TextBar(int x0, int y0, String text);
    void ReadyErase(int Trace);
    void ShowData(int NTraces);
    int  printText(int x, int y, int tsize, uint16_t TextColor, uint16_t BackColor, String Text);
    void printMenuText(int x, int y, int Tsize, uint16_t Color, boolean Selected, String text);
    void printTextOnly(int x, int y, int Tsize, uint16_t Color, String text);
    void drawVCursor(int x, uint16_t Color, boolean enabled = false);
};

MyScope scope;


//Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RESET);
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);


MyScope::MyScope() {
}

///////////////////////
//  320(w) x 240(h)  //
//  Constructor      //
///////////////////////
void MyScope::begin() {
  SPI.begin(14,12,13);  //  Need this to init SPI
  tft.begin();
  tft.fillScreen(ILI9341_BLACK);    
  pinMode(TFT_LED,OUTPUT);
  digitalWrite(TFT_LED,HIGH);
  tft.setRotation(1);
  width = tft.width();
  height = tft.height();
  leftmargin = 30;
  rightmargin = 40;
  topmargin = 60;
  bottommargin = 24;
  xdiv = 5;
  ydiv = 4;
  LeftYMax = 3.3;
  RightYMax = 3.3;
  startMHz = 0;
  endMHz = 50;
  XCursor = 75;
  traceWidth = tft.width()-leftmargin-rightmargin;
  TraceNames[0] = "T1";
  TraceNames[1] = "T2";
  TraceNames[2] = "T3";
  TraceNames[3] = "T4";
  Serial.println("TraceWidth: " + String(traceWidth));
  Serial.println("Width: " + String(width) + "  Height: " + String(height));
}


////////////////
//  Text Bar  //
////////////////
void MyScope::TextBar(int x0, int y0, String text) {
  int h = tft.height();
  tft.fillRoundRect(5,h-20,200,20,5, ILI9341_DARKGREY);
  tft.setCursor(8,h-17);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(1);
  tft.print(text);  
}

/////////////////
//  Print Text //
/////////////////
void MyScope::printMenuText(int x, int y, int Tsize, uint16_t Color, boolean Selected, String text) {
  if(Selected) tft.fillRoundRect(x,y-2,75,19,5, ILI9341_NAVY);
  else tft.fillRoundRect(x,y-2,75,19,5, ILI9341_BLACK);
  tft.setTextColor(Color);
  tft.setTextSize(Tsize);
  tft.setCursor(x,y);
  tft.print(" " + text);   
}

/////////////////
//  Print Text //
/////////////////
void MyScope::printTextOnly(int x, int y, int Tsize, uint16_t Color, String text) {
  tft.setTextColor(Color);
  tft.setTextSize(Tsize);
  tft.setCursor(x,y);
  tft.print(text);   
}
 
///////////////////
//  Erase Trace  //
///////////////////
boolean First = true;
void MyScope::EraseTrace(int Trace, int Offset) {
  int x, y, w = tft.width(), h = tft.height();
  if (~First) 
    for(x=1; x<width-leftmargin-rightmargin; x++) tft.drawLine(x+leftmargin-1,Offset-erase[Trace-1][x-1],x+leftmargin,Offset-erase[Trace-1][x],ILI9341_BLACK);  
    //for(x=1; x<w; x++) tft.drawLine(x+leftmargin-1,Offset-erase[Trace-1][x-1],x+leftmargin,Offset-erase[Trace-1][x],ILI9341_BLACK);  
  First = false;
}

//////////////////
//  Draw Trace  //
//////////////////
void MyScope::DrawTrace(int Trace, int Offset, uint16_t Color) {
  int x, y, w = tft.width(), h = tft.height();
  for(x=1; x<width-leftmargin-rightmargin; x++) tft.drawLine(x+leftmargin-1,Offset-data[Trace-1][x-1],x+leftmargin,Offset-data[Trace-1][x],Color);  
  ReadyErase(Trace);
}

double MyScope::pointToFreq(int p) {
  double Freq = startMHz + (((float)p/(float)traceWidth) * (endMHz-startMHz));
  return Freq;
}

int MyScope::printText(int x, int y, int tsize, uint16_t TextColor, uint16_t BackColor, String Text) {
  tft.setTextColor(TextColor, BackColor);
  tft.setTextSize(tsize);
  tft.setCursor(x,y);
  tft.print(Text);  
  return tft.getCursorX();
}

void MyScope::drawVCursor(int x, uint16_t Color, boolean enabled) {
  tft.drawFastVLine(leftmargin+lastXCursor,topmargin,height-bottommargin-topmargin,ILI9341_BLACK);  
  tft.drawFastVLine(leftmargin+x,topmargin,height-bottommargin-topmargin,Color);  
  //printText(5,18,1,ILI9341_WHITE,CURSORCOLOR," Freq: " + String(pointToFreq(x)) + "MHz ");
  lastXCursor = x;
}

void MyScope::ReadyErase(int Trace) {
  for(int i=0;i<400;i++) erase[Trace-1][i] = data[Trace-1][i];
}

uint16_t MyScope::TraceColor(int Trace) {
  switch(Trace) {
    case 1: return ILI9341_MAGENTA;
    case 2: return ILI9341_RED;   
    case 3: return ILI9341_GREEN;
    case 4: return ILI9341_BLUE;   
  } 
}

void MyScope::ShowAxis() {  
  //  Bottom Freq Scale
  /**
  uint16_t freqScaleColor = ILI9341_ORANGE;
  printTextOnly(10,height-15,1,freqScaleColor,String(startMHz,1)+"MHz");
  printTextOnly(leftmargin+((width-leftmargin-rightmargin)/2)-20,height-15,1,freqScaleColor,String(startMHz+((endMHz-startMHz)/2),1)+"MHz");
  printTextOnly(width-rightmargin-20,height-15,1,freqScaleColor,String(endMHz,1)+"MHz");
  tft.drawFastVLine(leftmargin+125,height-30,10,freqScaleColor);
  **/
  //  Left SWR Scale
  for(int i=0;i<=ydiv;i++)
    printTextOnly(leftmargin-25,height-bottommargin-3-(i*(height-topmargin-bottommargin)/ydiv),1,ILI9341_BLUE,String(((float)i/(float)ydiv)*LeftYMax,1));

  //  Right Ohms Scale
  for(int i=0;i<=ydiv;i++)
    printTextOnly(width-rightmargin+5,height-bottommargin-3-(i*(height-topmargin-bottommargin)/ydiv),1,ILI9341_GREEN,String(((float)i/(float)ydiv)*RightYMax,1));
}

void MyScope::DrawGrid() {
  int x, y;
  uint16_t ILI9341_VERYDARKGREY = tft.color565(64,64,64);     
  int pixpergrid = (width-leftmargin-rightmargin)/xdiv; 
  for(x=leftmargin; x<(leftmargin+(xdiv*pixpergrid))+1; x+=pixpergrid)  tft.drawFastVLine(x, topmargin, height-topmargin-bottommargin, ILI9341_DARKGREY);  
  for(y=topmargin; y<height; y+=(height-topmargin-bottommargin)/ydiv) tft.drawFastHLine(leftmargin, y, width-leftmargin-rightmargin, ILI9341_DARKGREY);  
  
  //drawVCursor(XCursor, CURSORCOLOR, true);
  ShowAxis();
}

void MyScope::ShowData(int NTraces) {
  DrawGrid();
  int16_t col = 5;  
  for(int i=1;i<=NTraces;i++) {
    EraseTrace(i,height-bottommargin);
    DrawTrace(i,height-bottommargin,TraceColor(i)); 
    col = printText(col,32,1,ILI9341_BLACK,TraceColor(i)," " + TraceNames[i-1] + ":" + String(cursorValues[i-1]) + " ") + 5;
  }
}

#endif

