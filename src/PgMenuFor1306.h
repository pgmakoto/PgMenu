
//#include "PgMenuFor1306.h"

#ifndef PgMENUFOR1306_H
#define PgMENUFOR1306_H

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include "PgStick.h"
#include "PgMenu.h"


//必要な初期化(static)　https://qiita.com/omuRice/items/3c40e8dde19e276ccacf

MenuItem* MenuPage::editItem = NULL;
int MenuPage::editLeft = 5;
int MenuPage::editTop = 0;
int MenuPage::editWidth = 10;
int MenuPage::editHeight = 10;

int MenuPage::menu_left = 10;
int MenuPage::menu_top = 8;
int MenuPage::menu_width = 110;
int MenuPage::menu_height = 55;
int MenuPage::menu_disptop = 0;
int MenuPage::CharW = 6;
int MenuPage::CharH = 10;

//中身の描画
void MenuPage::drawEditMenu() {
  //draw edit
  if (editItem != NULL) {

    //editTop   //editHeight = ;;;
    //編集中 print///////////////////////////////////////////////////////////////////////////////////////////
    if (editItem->type == NUM) {
      //中身の描画
      char cstr[16];
      sprintf(cstr, "%6d", editvalue);
      display.setCursor(editLeft + 3, editTop + 5);
      display.print(cstr);
      //選択している桁のカーソル表示
      display.fillRect(editLeft + (5 - edititem_col) * CharW + 2, editTop + 3, CharW + 1, CharH + 1, SSD1306_INVERSE);
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    if (editItem->type == CHECK) {

      //選択されているカーソル表示
      byte selState = 0;  //only print
        selState = 1;                  ////SELECTED
        if (X->stateprev) selState++;  ////Pushed
        if (X->state_hold) selState++;
        //  display.fillRoundRect(0, ypos - 2, 127, CharH + 3, 3, SSD1306_INVERSE);  //right to left
        //else
        //  display.fillRoundRect(1, ypos - 1, 125, CharH + 1, 3, SSD1306_INVERSE);  //right to left

      if (editbool) {
        printItem(editLeft + 1, editTop + 2, editWidth - 2, CharH + 1, ">*<",  selState) ;
        printItem(editLeft + 1, editTop +CharH+ 2, editWidth - 2, CharH + 1, "   ",  0) ;
      }else{
        printItem(editLeft + 1, editTop + 2, editWidth - 2, CharH + 1, " * ", 0) ;
        printItem(editLeft + 1, editTop +CharH+ 2, editWidth - 2, CharH + 1, "> <", selState) ;
      }
      return;
      //中身の描画
      display.setCursor(editLeft + 2, editTop + 2);
      display.print(" * ");
      //  display.setCursor(50, editTop + CharH);
      if (editbool) {
        display.setCursor(editLeft + 2, editTop + 2);
        display.print("[ ]");
      } else {
        display.setCursor(editLeft + 2, editTop + CharH + 2);
        display.print("[ ]");
      }
    }
    //\\\\\\\\\\\\\\\\\\\\\\\\\\\\///////////////////////////////////////////////////////////////////////////

    return;
  }
}


void MenuPage::drawSelectItems(MenuItem* items) {
  int cwid = (menu_width - 5) / CharW;
  int i = 0;
  while (true) {
    MenuItem* item = &items[i];
    int ypos = (i * CharH) - menu_disptop;
    if ((ypos >= 0) && (ypos + CharH + 4 < menu_height)) {
      //１行の内容表示
      String strItem = "";
      String strContent = "";
      bool chk = *(bool*)item->command;
      if (editbool) {
        printItem(menu_left + 1, menu_top + ypos + 1, menu_width - 2, CharH + 1, ">*<", 0);
        printItem(menu_left + 1, menu_top + ypos + 1, menu_width - 2, CharH + 1, "   ", 0);
      } else {
        printItem(menu_left + 1, menu_top + ypos + 1, menu_width - 2, CharH + 1, " * ", 0);
        printItem(menu_left + 1, menu_top + ypos + 1, menu_width - 2, CharH + 1, "> <", 0);
      }
    }
    i++;
  }
}

//枠の描画
void MenuPage::drawMenuWindow(int x, int y, int w, int h) {
  display.fillRoundRect(x, y, w, h, 2, SSD1306_BLACK);
  display.drawRoundRect(x + 1, y + 1, w - 2, h - 2, 2, SSD1306_WHITE);
}
//描画終了（表示更新）
void MenuPage::menuDisplay() {
  display.display();
}
// //MenuItemの描画 (1行)
// typedef enum mENUTYPE {
//   DOCOMMAND,
//   NUM,
//   SELECT,
//   CHECK,
//   PAGE,
//   PAGEEND
// } MENUTYPE;

void MenuPage::printItem(int x, int y, int w, int h, String text, byte selState) {
  display.setCursor(x + 2, y + 3);
  display.print(text);
  if (selState == 1)  //SELECTED
  {
    display.fillRoundRect(x + 2, y + 1, w - 4, h, 3, SSD1306_INVERSE);  //right to left
  } else if (selState == 2)                                             //Pushed
  {
    display.fillRoundRect(x+1, y, w-2, h + 2, 4, SSD1306_INVERSE);  //right to left
  } else if (selState == 3)                                     //Pushed
  {
    display.drawRoundRect(x + 2, y + 1, w - 4, h, 3, SSD1306_INVERSE);
  }
}









#endif