
#include "Scope.h"
//#include "RotaryMenu.h"
#include "TouchMenu.h"
#include <Preferences.h>

#define ESP32 1

#ifdef ESP8266
#define CS_PIN D3
#endif

#ifdef ESP32

#define DDS_CLK 17
#define DDS_LOAD 16
#define DDS_DATA 4
#define DDS_RESET 2
#define FQ_UD DDS_LOAD
#define SDAT DDS_DATA
#define SCLK DDS_CLK

#define LED 15
#endif

//  Put on GIT

#define A1  33
#define A2  25
#define V1  32

#define TotalSamples 125


Preferences preferences;

/////////////
//  Setup  //
/////////////
void setup() {
  Serial.begin(115200);
  Serial.setTimeout(100);
  delay(500);
  Serial.println("Power Monitor Test start test new!"); 

  scope.begin();
  
  Serial.println("Runing....");  

  //StoreTraceOffsets();
  RecallTraceOffsets();
  
  MyTouchMenuSelection selection;
//  selection.setPosition(20,20);
//  selection.addSelection("Scan",ILI9341_GREEN,ILI9341_BLACK);  
//  selection.addSelection("Reset",ILI9341_YELLOW,ILI9341_BLACK);  
//  selection.addSelection("Oscilloscope",ILI9341_RED,ILI9341_BLACK);  
//  selection.addSelection("TEST",ILI9341_RED,ILI9341_BLACK);  
//  selection.showSelections(true);
  
  touchMenu.addButton(10,10,100,40,"Oscilloscope",ILI9341_RED,ILI9341_WHITE);  
  touchMenu.addButton(10,70,100,40,"Scan",ILI9341_GREEN,ILI9341_BLACK);  
  touchMenu.addButton(10,170,100,40,"Reset",ILI9341_YELLOW,ILI9341_BLACK);  
  touchMenu.addButton(220,10,75,40,"Prev",ILI9341_MAROON,ILI9341_WHITE);  
  touchMenu.addButton(220,70,75,40,"Next",ILI9341_MAROON,ILI9341_WHITE);  
}

void RecallTraceOffsets() {
  preferences.begin("my-app");
  Serial.println("");
  Serial.println("Stored Offsets:");
  Serial.println("V1OFF: " + String(preferences.getFloat("V1OFF",0)));
  Serial.println("V1OFF: " + String(preferences.getFloat("A1OFF",0)));
  Serial.println("V1OFF: " + String(preferences.getFloat("A2OFF",0)));
  preferences.end();  

  Serial.println("");
  Serial.println("Measured Offsets:");
  Serial.println("V1 Baseline: " + String(GetBaseline(V1)));
  Serial.println("A1 Baseline: " + String(GetBaseline(A1)));
  Serial.println("A2 Baseline: " + String(GetBaseline(A2)));
}

void StoreTraceOffsets() {
  preferences.begin("my-app");
  preferences.clear();
  preferences.putFloat("V1OFF",GetBaseline(V1));
  preferences.putFloat("A1OFF",GetBaseline(A1));
  preferences.putFloat("A2OFF",GetBaseline(A2));
  preferences.end();  
}

int GetBaseline(int Channel) {
  int SampleDelay = round(((((float)1 / (float)60) / TotalSamples) * 1e6) * 2);
  int Average = GetAverage(Channel, 100, SampleDelay);
  return Average;
}

int GetAverage(int ADCN, int Samples, int SampleDelay) {
  long Total = 0;
  for (int i = 1; i <= Samples; i++) {
    Total = Total + analogRead(ADCN);
    delayMicroseconds(SampleDelay);
  }
  return round(Total / Samples);
}

void WaitingForRisingEdge(int ADCN, int SampleDelay) {
  int S1, S2;
  S1 = 0;
  S2 = 0;
  int Average = GetAverage(ADCN, 200, SampleDelay);
  while (S2 <= S1) {
    while (analogRead(ADCN) > Average);
    while (analogRead(ADCN) < Average);
    S1 = analogRead(ADCN);
    delayMicroseconds(3 * SampleDelay);
    S2 = analogRead(ADCN);
  }
}



void DoMonitor() {
  int Samples1[TotalSamples];
  int Samples2[TotalSamples];
  int Samples3[TotalSamples];


  //  Calculate Sample Delay for 2 Cycles, 60Hz, TotalSamples
  int SampleDelay = round(((((float)1 / (float)60) / TotalSamples) * 1e6) * 2);

  //  Wait for Go Command that contains Threshold Above Average
  while (Serial.available() == 0);
  int Threshold = Serial.parseInt();

  //  Obtain Average and Set Threshold To It
  int Average = GetAverage(V1, 2*TotalSamples, SampleDelay);
  Threshold = Average;

  //  Find Trigger Point
  WaitingForRisingEdge(V1, SampleDelay);
  while (analogRead(V1) < Threshold);

  //  Collect Samples
  for (int i = 0; i < TotalSamples; i++) {
    long tt1 = 0;
    long tt2 = 0;
    long tt3 = 0;
    long t = micros();
    int n = 0;
    while(micros()<(t+SampleDelay)) {
      tt1 = tt1 + analogRead(V1);
      tt2 = tt2 + analogRead(A1);
      tt3 = tt3 + analogRead(A2);
      n++;
    }
    Samples1[i] = round(tt1 / n);
    Samples2[i] = round(tt2 / n);
    Samples3[i] = round(tt3 / n);
    //delayMicroseconds(SampleDelay);
  }

  for (int i = 0; i < TotalSamples; i++) {
    Serial.print(String(Samples1[i]) + ",");
  }
  for (int i = 0; i < TotalSamples; i++) {
    Serial.print(String(Samples2[i]) + ",");
  }
  for (int i = 0; i < TotalSamples; i++) {
    Serial.print(String(Samples3[i]) + ",");
  }
  Serial.println("");
}

void DoLCD() {
  int Samples1[TotalSamples];
  int Samples2[TotalSamples];
  int Samples3[TotalSamples];

  //  Calculate Sample Delay for 2 Cycles, 60Hz, TotalSamples
  int SampleDelay = round(((((float)1 / (float)60) / TotalSamples) * 1e6) * 2);

  int Threshold = 1;

  //  Obtain Average and Set Threshold To It
  int Average = GetAverage(V1, 2*TotalSamples, SampleDelay);
  Threshold = Average;

  //  Find Trigger Point
  WaitingForRisingEdge(V1, SampleDelay);
  while (analogRead(V1) < Threshold);

  scope.TraceNames[0] = "V1";
  scope.TraceNames[1] = "A1";
  scope.TraceNames[2] = "A2"; 
  scope.RightYScale = (float)(scope.height-scope.topmargin-scope.bottommargin) / scope.RightYMax;
  scope.LeftYScale  = (float)(scope.height-scope.topmargin-scope.bottommargin) / scope.LeftYMax;

  //  Collect Samples
  for (int i = 0; i < TotalSamples; i++) {
    long tt1 = 0;
    long tt2 = 0;
    long tt3 = 0;
    long t = micros();
    int n = 0;
    while(micros()<(t+SampleDelay)) {
      tt1 = tt1 + analogRead(V1);
      tt2 = tt2 + analogRead(A1);
      tt3 = tt3 + analogRead(A2);
      n++;
    }
    Samples1[i] = round(tt1 / n);
    Samples2[i] = round(tt2 / n);
    Samples3[i] = round(tt3 / n);
  }

  for(int i=0;i<TotalSamples;i++) { 
    scope.data[0][(i*2)]   = round(((float)(Samples1[i] / (float)4095) * (float)3.5) * (float)scope.LeftYScale);
    scope.data[0][(i*2)+1] = round(((float)(Samples1[i] / (float)4095) * (float)3.5) * (float)scope.LeftYScale);
  }
  for(int i=0;i<TotalSamples;i++) { 
    scope.data[1][(i*2)]   = round(((float)(Samples2[i] / (float)4095) * (float)3.5) * (float)scope.LeftYScale);
    scope.data[1][(i*2)+1] = round(((float)(Samples2[i] / (float)4095) * (float)3.5) * (float)scope.LeftYScale);
  }
  for(int i=0;i<TotalSamples;i++) { 
    scope.data[2][(i*2)]   = round(((float)(Samples3[i] / (float)4095) * (float)3.5) * (float)scope.LeftYScale);
    scope.data[2][(i*2)+1] = round(((float)(Samples3[i] / (float)4095) * (float)3.5) * (float)scope.LeftYScale);
  }
  scope.ShowData(3);
}


////////////
//  Loop  //
////////////
void loop(void) {
  //if(Serial.available() != 0) DoMonitor();
  
  if(touchMenu.centerTouch()) {
    tft.fillScreen(ILI9341_BLACK);
    touchMenu.showButtons(true);
    long start = millis();
    while ((millis()-start) < 2000) {
      int button = touchMenu.checkButtonPressed();    
      if(button >= 0) start = millis();
    }
    tft.fillScreen(ILI9341_BLACK);
  } else {
    DoLCD();
  }
}


