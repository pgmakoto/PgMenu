#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_SSD1306.h"
#include "Fonts/FreeSans12pt7b.h"

#include <EEPROM.h>
/////////////////////////////////   SSD1306 OLED Display   /////////////////////////////////////////////////
#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels
#define OLED_CS PB14
#define OLED_DC PB11     //
#define OLED_RESET PC14  //

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, OLED_DC, OLED_RESET, OLED_CS);
/*SSD1306 0.96inch  SSD1309 2.42inch
              PA5 PA7 PB15 PC13 PB14
    _________________________________
    |  GND VDD SCK SDA RES DC  CS   |
    |   _________________________   |
    |   |                       |   |
    |   |                       |   |
    |   |      128 x 64         |   |
    |   |         0.96inch      |   |
    |   |         2.42inch      |   |
    |   |                       |   |
    |   _________________________   |
    |                               |
    ---------------------------------*/
/////\\\\\/////\\\\\/////\\\\\/////\\\\\/////\\\\\/////  menu  /////\\\\\/////\\\\\/////\\\\\/////\\\\\/////\\\\\/////
#include "PgStick.h"
PgStick StickX(PA1);
PgStick StickY(PA2);
#include "PgMenuFor1306.h"
PgStick* MenuPage::X = &StickX;  //Menu に　PgStickを登録
PgStick* MenuPage::Y = &StickY;

bool TestMenu = false;
bool State_Item1 = false;
bool State_Item2 = false;
bool State_Item3 = false;

int HOLDOFFTIME = 300;
bool CutEnable[6] = { true, true, true, true, true, true };
int CUTTIME[6] = { 1, 120, 80, 60, 60, 55 };  //N 1 2 3 4 5   カット時間

void doReset() {
  for (int i = 0; i < 6; i++)
    CutEnable[i] = false;
}

void Full_Menu() {
  TestMenu = false;
  MenuPage::setRect(-2, -2, 128 + 4, 64 + 4);
}
void Def_Menu() {
  TestMenu = false;
  MenuPage::setRect(5, 5, 118, 54);
}

void saveCUTTIME() {  //(MenuCommand)
  //1stTime
  int eeAddress = 128;  //128 to 199  2*9*4 72
  short step = sizeof(short);
  short val = 0;
  for (int i = 0; i < 6; i++) {
    val = (short)CUTTIME[i];
    EEPROM.put(eeAddress, val);
    eeAddress += step;
  }
}
void loadCUTTIME() {  //(MenuCommand)
  //1stTime
  int eeAddress = 128;  //128 to 199  2*9*4 72
  short step = sizeof(short);
  short val = 0;
  for (int i = 0; i < 6; i++) {
    EEPROM.get(eeAddress, val);
    CUTTIME[i] = (int)val;
    eeAddress += step;
  }
}
// CUTTIME = { 1, 120, 80, 60, 60, 55 };
void defaultCUTTIME() {  //(MenuCommand)
  CUTTIME[0] = 1;
  CUTTIME[1] = 120;
  CUTTIME[2] = 80;
  CUTTIME[3] = 60;
  CUTTIME[4] = 60;
  CUTTIME[5] = 55;
}

MenuItem CutTimeitems[] = {
  MenuItem("default", (MenuCommand*)defaultCUTTIME),
  MenuItem("load", (MenuCommand*)loadCUTTIME),
  MenuItem("CUT1>2", &CUTTIME[1]),
  MenuItem("CUT2>3", &CUTTIME[2]),
  MenuItem("CUT3>4", &CUTTIME[3]),
  MenuItem("CUT4>5", &CUTTIME[4]),
  MenuItem("CUT5>6", &CUTTIME[5]),
  MenuItem("Save", (MenuCommand*)saveCUTTIME),
  MenuItem("end", PAGEEND)  //必ず最後に入れる
};
MenuItem StickSETTINGitems[] = {
  MenuItem("XHOLDTIME", &StickX.holdtime),
  MenuItem("XCLICKTIME", &StickX.clicktime),
  MenuItem("YHOLDTIME", &StickY.holdtime),
  MenuItem("YCLICKTIME", &StickY.clicktime),
  MenuItem("Item1", &State_Item1),
  MenuItem("Item2", &State_Item2),
  MenuItem("Item3", &State_Item3),
  MenuItem("end", PAGEEND)  //必ず最後に入れる
};
MenuItem CutEnableitems[] = {
  MenuItem("CUT 1", &CutEnable[1]),
  MenuItem("CUT 2", &CutEnable[2]),
  MenuItem("CUT 3", &CutEnable[3]),
  MenuItem("CUT 4", &CutEnable[4]),
  MenuItem("CUT 5", &CutEnable[5]),
  MenuItem("end", PAGEEND)  //必ず最後に入れる
};
MenuItem Selectsitems[] = {
  MenuItem("SEL 0", 0),
  MenuItem("SEL 1", 1),
  MenuItem("SEL 2", 2),
  MenuItem("end", PAGEEND)  //必ず最後に入れる
};
MenuItem page0items[] = {
  MenuItem("TestMenu", &TestMenu),
  MenuItem("Full_Menu", (MenuCommand*)Full_Menu),
  MenuItem("Def_Menu", (MenuCommand*)Def_Menu),
  MenuItem("State_Item1", &State_Item1),
  MenuItem("HOLDOFFTIME", &HOLDOFFTIME),
  MenuItem("STICKSETTING", StickSETTINGitems),
  MenuItem("CutTime", CutEnableitems),
  MenuItem("Select3", Selectsitems, 2),

  MenuItem("DoRESET ", (MenuCommand*)doReset),
  MenuItem("end", PAGEEND)  //必ず最後に入れる
};

MenuPage page0 = MenuPage(page0items);
MenuPage* MenuPage::menucurpage = &page0;

/////\\\\\/////\\\\\/////\\\\\/////\\\\\/////\\\\\/////  menu  /////\\\\\/////\\\\\/////\\\\\/////\\\\\/////\\\\\/////

void setup() {
  Serial.begin(115200);
  
  pinMode(OLED_RESET, OUTPUT);  //  //
  digitalWrite(OLED_RESET, LOW);
  while (millis() < 200) {
  }
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC)) {
    while (!Serial && millis() < 2000)
      ;
    Serial.println(F("SSD1306 allocation failed"));
    while (1) {}  // Don't proceed, loop forever
  }



  //page0.begin(page0items);

  // Clear the buffer
  display.clearDisplay();  //----------------------------------
  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.setRotation(0);  //rotates text on OLED 1=90 degrees, 2=180 degrees
  display.display();
  delay(20);  // Pause for 2 seconds



  display.setCursor(0, 0);
  display.setFont(&FreeSans12pt7b);
  display.setTextSize(1);                 // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_INVERSE);  // Draw white text

  display.setCursor(0, 20);
  display.println("QuiqShifter");
  display.display();
  display.setFont();
}

int wd = 100;
int ht = 100;
bool wdp = false;
bool htp = false;
int step = 4;

int lap = 0;
int fps;
int fpscnt;
unsigned long fpstmprev = 0;

void loop() {
  // put your main code here, to run repeatedly:
  //メニューのテスト用
  if (TestMenu) {
    if (step-- < 0) {
      if (wdp) {
        if (wd++ > 120) wdp = false;
      } else {
        if (wd-- < 60) wdp = true;
      }
      if (htp) {
        if (ht++ > 64) htp = false;
      } else {
        if (ht-- < 30) htp = true;
      }
      step = 4;
      MenuPage::setRect(50 - wd / 2, 30 - ht / 2, wd, ht);
    }
  }
  MenuPage::update();  //メニューの更新

  /*    fps calc     */
  unsigned long curtm = millis();
  lap = curtm - fpstmprev;
  if (lap > 100 || fpscnt++ > 10) {
    fpstmprev = curtm;
    fps = (fpscnt * 1000) / lap;
    fpscnt = 0;
  }
}
