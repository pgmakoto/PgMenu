/*
#include "PgMenu.h"
    Copyright (c) 2024-2025 ,plusg Makoto kuwabara
*/
#ifndef PgMENU_H
#define PgMENU_H

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include "PgStick.h"
#include "PgBUTTON.h"

class MenuItem;
class MenuPage;

byte edititem_col = 0;  //数値設定用桁位置
int editvalue;          //数値設定用
bool editbool;          //ブール設定用

typedef enum mENUTYPE {
  DOCOMMAND,
  NUM,
  SHORTNUM,
  FNUM,
  SELECT,
  CHECK,
  PAGE,
  NEWPAGE,
  EDITPAGE,
  VALUE,
  PAGEEND
} MENUTYPE;

typedef void (*MenuCommand)(void);
class MenuPage {
private:

public:

  MenuPage* parentPage = NULL;  //戻るときの親ページ情報
  //int value = 0;
  int menu_row = 0;  //選択中のアイテム番号(カーソル位置)
  int itemnum = 0;   //アイテム数
  MenuItem* items;   //アイテムのリストが入る
  int value = 0;     //
  static MenuPage* firstPage;
  static unsigned long prevtime;
  static unsigned int menulife;
  static bool live;

  static MenuPage* menucurpage;
  //エディット用サブ画面
  static MenuItem* editItem;
  static int editLeft;
  static int editTop;
  static int editWidth;
  static int editHeight;
  static PgStick* X;
  static PgStick* Y;

  static int menu_left;
  static int menu_top;
  static int menu_width;
  static int menu_height;
  static int menu_disptop;
  static int CharW;
  static int CharH;

  static void setChar(int width, int height) {
    CharW = width;
    CharH = height;
    adjustDisp();
  }

  static void setRect(int x, int y, int width, int height) {
    menu_left = x;
    menu_top = y;
    menu_width = width;
    menu_height = height;
    adjustDisp();
  }

  static void adjustDisp() {
    int menuItemBottom = menucurpage->itemnum * CharH + 5;
    if (menuItemBottom < menu_disptop + menu_height) menu_disptop--;
    int cursoltop = menucurpage->menu_row * CharH;
    if (cursoltop < menu_disptop) menu_disptop--;
    if (cursoltop + CharH + 5 > menu_disptop + menu_height) menu_disptop++;
  }

  // MenuPage(const MenuPage &q) : parentPage(q.parentPage), items(q.items), itemnum(q.itemnum) {}
  MenuPage() {}
  MenuPage(MenuItem* _items);

  static void wakeUp() {
      prevtime = millis();
	  MenuPage::live=true;
      MenuPage::editItem = NULL;

      while (MenuPage::menucurpage->parentPage != NULL) {
          MenuPage* parentmenupage = MenuPage::menucurpage->parentPage;

          delete MenuPage::menucurpage;
          MenuPage::menucurpage = parentmenupage;  //戻る位置を復元
      }
  }
  static bool isSleep() {
	  return !MenuPage::live;
  }
  void begin(MenuItem* _items);
  // const MenuPage operator*(const MenuPage &q) const { return MenuPage(*this) *= q; }

  static void update(void);
  static void drawMenu(void);
  static void drawEditMenu(void);
  static void drawMenuWindow(int x, int y, int w, int h);
  static void clearMenuWindow();
  static void printItem(int left, int y, int w, int h, String text, byte selState);
  static void menuDisplay();
  static void drawSelectItems(MenuItem* items);
};

class MenuItem {
public:
  MENUTYPE type;
  String text;
  //MenuCommand *command = NULL;
  void* command = NULL;
  byte refrow = 0;  //SELECT でのみ使用している　ほかの方法？

  MenuItem(String _text, MENUTYPE _type) {
    text = _text;
    type = _type;
  }
  MenuItem(String _text, int* _num) {
      text = _text;
      type = NUM;
      command = _num;
  }
  MenuItem(String _text, short* _num) {
      text = _text;
      type = SHORTNUM;
      command = _num;
  }
  MenuItem(String _text, float* _num) {
      text = _text;
      type = FNUM;
      command = _num;
  }
  MenuItem(String _text, bool* _chk) {
    text = _text;
    type = CHECK;
    command = _chk;
  }
  MenuItem(String _text, MenuCommand* _command) {
    text = _text;
    type = DOCOMMAND;
    command = _command;
  }
  MenuItem(String _text, MenuPage* _page) {
    text = _text;
    type = PAGE;
    command = _page;
  }
  MenuItem(String _text, MenuItem* _items) {
    text = _text;
    type = NEWPAGE;
    command = _items;
  }
  MenuItem(String _text, MenuItem* _pageofselect, int index) {  //for SELECT　index = default selected Row
    text = _text;
    type = SELECT;
    command = _pageofselect;
    refrow = index;
  }
  MenuItem(String _text, int index) {  //for SELECT
    text = _text;
    type = VALUE;
    command = (void*)index;
  }

  void push();
  void back();
};


MenuPage::MenuPage(MenuItem* _items) {
  firstPage = this;
  items = _items;
  int i = 0;
  for (i = 0; i < 20; i++) {
    if (items[i].type == PAGEEND) break;
  }
  itemnum = i;  //_itemnum;
}

void MenuPage::begin(MenuItem* _items) {
  firstPage = this;
  items = _items;
  int i = 0;
  for (i = 0; i < 20; i++) {
    if (items[i].type == PAGEEND) break;
  }
  itemnum = i;  //_itemnum;
}

void MenuItem::push() {
  if (type == DOCOMMAND) {
    if (command != NULL) {
      (void)(*((MenuCommand)command))();
    }
  }
  else if (type == NUM) {
      //Edit mode
      if (MenuPage::editItem == NULL) {
          MenuPage::editItem = this;
          editvalue = *(int*)command;
      }
  }

  else if (type == SHORTNUM) {
      //Edit mode
      if (MenuPage::editItem == NULL) {
          MenuPage::editItem = this;
          editvalue = *(short*)command;
      }
  }
 else if (type == SELECT) {  //サブページにするべき
                                //Edit mode
    MenuPage* newmenucurpage = new MenuPage((MenuItem*)command);  //新しく表示を行うメニュー itemlist
    newmenucurpage->parentPage = MenuPage::menucurpage;           //戻る位置を保存
    MenuPage::menucurpage = newmenucurpage;
    //  MenuPage::editItem  呼び出し元のparent Item
    //  MenuPage::menucurpage　

  } else if (type == VALUE) {  //SELECT の次
    //Edit mode
    MenuPage* parentmenupage = MenuPage::menucurpage->parentPage; 
    MenuItem* parentmenuitem = &parentmenupage->items[parentmenupage->menu_row]; 
    if ((parentmenuitem != NULL) && (parentmenuitem->type == SELECT)) {
      parentmenuitem->refrow = MenuPage::menucurpage->menu_row;
    }
  } else if (type == CHECK) {
    //Edit mode
    if (MenuPage::editItem == NULL) {
      MenuPage::editItem = this;
      editbool = *(bool*)command;
    }
  } else if (type == PAGE) {
    MenuPage* newmenucurpage = (MenuPage*)command;       //新しく表示を行うメニュー
    newmenucurpage->parentPage = MenuPage::menucurpage;  //戻る位置を保存
    MenuPage::menucurpage = newmenucurpage;
  } else if (type == NEWPAGE) {                                   //動的
    MenuPage* newmenucurpage = new MenuPage((MenuItem*)command);  //新しく表示を行うメニュー itemlist
    newmenucurpage->parentPage = MenuPage::menucurpage;           //戻る位置を保存
    MenuPage::menucurpage = newmenucurpage;
  }
}

void MenuItem::back() {
  if (MenuPage::editItem != NULL) {  //編集中の場合はキャンセル扱い
    MenuPage::editItem = NULL;
    return;
  }

  MenuPage* parentmenupage = MenuPage::menucurpage->parentPage;  //   (MenuPage*)command;

  if (type == DOCOMMAND) {
  }
  else if (type == NUM) {
  }
  else if (type == SHORTNUM) {
  } else if (type == SELECT) {
  } else if (type == CHECK) {
  } else if (type == PAGE) {
  }

  if (parentmenupage != NULL) {
    delete MenuPage::menucurpage;
    MenuPage::menucurpage = parentmenupage;  //戻る位置を復元
  }
  else {
      //メニューの終了
      MenuPage::prevtime = millis() - MenuPage::menulife;
	  
  }
}
/*
// //MenuItemの描画
  // typedef enum mENUTYPE {
  //   DOCOMMAND,
  //   NUM,
  //   SELECT,
  //   CHECK,
  //   PAGE,
  //   PAGEEND
  // } MENUTYPE;
*/

void MenuPage::update() {
  //        up  Y+ (prev)
  //(back)left X- X+ right(set)
  //       down Y- (next)


    int res = X->update();
    res += Y->update();
    // 
    //PgStick auto sleep
    unsigned long now = millis();
    if (!MenuPage::live ) {
        if (X->Hold()) {
            wakeUp();
        }
		return;
    }
	else if(now - prevtime > menulife){
		MenuPage::live=false;
		MenuPage::clearMenuWindow(); 
		return;
	}
    else if (res) {
        prevtime = now; 
    }
	
	

  if (editItem != NULL && (editItem->type != SELECT)) {  //SELECTはメニューのデフォルト機能を使用
    //数値入力編集中 print///////////////////////////////////////////////////////////////////////////////////////////
    if (editItem->type == NUM) {
      //桁の選択
      if (X->Hold()) {
        if (X->dir) {
          *(int*)(editItem->command) = editvalue;
        }
        editItem = NULL;
        return ;
      }
      if (X->Click()) {
        if (!X->dir) {
          if (edititem_col < 5) edititem_col++;
        } else {
          if (edititem_col > 0) edititem_col--;
        }
      }
      if (Y->Click()) {
        if (!Y->dir) {
          editvalue -= pow(10, edititem_col);
        } else {
          editvalue += pow(10, edititem_col);
        }
      }
      editWidth = CharW * 6 + 7;
      editHeight = CharH + 7;
    }
    if (editItem->type == SHORTNUM) {
        //桁の選択
        if (X->Hold()) {
            if (X->dir) {
                *(short*)(editItem->command) = (short)editvalue;
            }
            editItem = NULL;
            return ;
        }
        if (X->Click()) {
            if (!X->dir) {
                if (edititem_col < 4) edititem_col++;
            }
            else {
                if (edititem_col > 0) edititem_col--;
            }
        }
        if (Y->Click()) {
            if (!Y->dir) {
                editvalue -= pow(10, edititem_col);
            }
            else {
                editvalue += pow(10, edititem_col);
            }
        }
        editWidth = CharW * 6 + 7;
        editHeight = CharH + 7;
    }
    ///チェック入力　/////////////////////////////////////////////////////////////////////////////////////////////////////
    if (editItem->type == CHECK) {
      if (X->Hold()) {
        if (X->dir) {
          *(bool*)(editItem->command) = editbool;
        }
        editItem = NULL;
        return ;
      }
      if (X->Click() && (!X->dir)) {
        editItem = NULL;
        return ;
      }
      if (Y->Click()) {
        editbool = Y->dir;
      }
      editWidth = CharW * 3 + 6;
      editHeight = CharH * 2 + 7;
    }
    //高さ調整
    //editTop = menu_top + (menucurpage->menu_row * CharH) - menu_disptop;
    if (editTop + editHeight > menu_top + menu_height) editTop = menu_height + menu_top - editHeight;
    if (editTop < menu_top) editTop = menu_top;

    editLeft = menu_left + menu_width - editWidth;

    //////////////////////////////////////////////////////////////////////////////////////////////////////////
    //枠の描画
    drawMenuWindow(editLeft, editTop, editWidth, editHeight);
    //中身の描画
    drawEditMenu();
    //描画終了
    menuDisplay();
    return ;
  }

  //--------------------------------------------------------
  if (X->Hold()) {
    if (!X->dir) {
      menucurpage->items[menucurpage->menu_row].back();
    } else {
      menucurpage->items[menucurpage->menu_row].push();
      editTop = (menucurpage->menu_row * CharH) + menu_top - menu_disptop;  //サブエディットメニューの場合使用
    }
  }
  if (Y->Click()) {
    if (Y->dir) {
      menucurpage->menu_row--;
      if (0 > menucurpage->menu_row) menucurpage->menu_row = menucurpage->itemnum - 1;
    } else {
      menucurpage->menu_row++;
      if (menucurpage->itemnum - 1 < menucurpage->menu_row) menucurpage->menu_row = 0;
    }
  }


  adjustDisp();
  //Draw menu items & cursol
  //item select Menu
  //display.clearDisplay();
  //display.setCursor(0 ,0);

  drawMenuWindow(menu_left, menu_top, menu_width, menu_height);
  //-------------------------------
  int cwid = (menu_width - 5) / CharW;
  for (int i = 0; i < menucurpage->itemnum; i++) {
    MenuItem* item = &menucurpage->items[i];
    int ypos = (i * CharH) - menu_disptop;
    if ((ypos >= 0) && (ypos + CharH + 4 < menu_height)) {
      //１行の内容表示
      String strItem = "";
      String strContent = "";

      if (item->type == DOCOMMAND) {
        strItem = "(";
        strItem += String(item->text);
        strItem += ")";
      }
      else if (item->type == NUM) {
          strItem = " ";
          strItem += String(item->text);
          strContent = "=";
          int numeric = *(int*)item->command;
          strContent += String(numeric);
      }
      else if (item->type == SHORTNUM) {
          strItem = " ";
          strItem += String(item->text);
          strContent = "=";
          short numeric = *(short*)item->command;
          strContent += String(numeric);
      }
      else if (item->type == SELECT) {
        strItem = " ";
        strItem += String(item->text);
        strItem += "=";
        MenuItem* refitems = (MenuItem*)item->command;
        //  int numeric = *(int*)(refitems[item->refrow].command);  //セレクトページのデータの中身
        //これはINTの場合！！！いろいろ設定できるようにする       item.refrow;//設定されている値の行番号
        //  strContent += "(" + String(numeric) + ")";

        strContent += "<" + refitems[item->refrow].text + ">";

      } else if (item->type == VALUE) {

        MenuPage* parentmenupage = MenuPage::menucurpage->parentPage;
        if (parentmenupage != NULL) {
          if (parentmenupage->items[parentmenupage->menu_row].refrow == i) {
            strItem = "<"+String(item->text)+">";
          } else {
            strItem = " "+String(item->text)+" ";
          }
          strItem += "---------";

          int valueOfItem = (int)item->command;
          //  int numeric = *(int*)(refitems[item->refrow].command);  //セレクトページのデータの中身
          //これはINTの場合！！！いろいろ設定できるようにする       item.refrow;//設定されている値の行番号
            strContent += "(" + String(valueOfItem) + ")";

        //  strContent += "" + refitems[item->refrow].text + "";
        }
      } else if (item->type == CHECK) {
        bool chk = *(bool*)item->command;
        strItem = " ";
        strItem += String(item->text);
        strItem += " ";
        if (chk) strContent = "(*)";
        else strContent = "(_)";
      } else if (item->type == PAGE) {
        strItem = " ";
        strItem += String(item->text);
        strItem += ">";
      } else if (item->type == NEWPAGE) {
        strItem = " ";
        strItem += String(item->text);
        strItem += ">>";
      }
      if (strContent.length() > cwid) {
        strItem = " ";
        strContent = strContent.substring(strContent.length() - cwid, cwid);
      } else if (strItem.length() + strContent.length() > cwid) {
        strItem = strItem.substring(0, cwid - strContent.length());
      } else {
        strItem = (strItem + "              ").substring(0, cwid - strContent.length());
      }
      strItem += strContent;
      //選択されているカーソル表示
      byte selState = 0;  //only print
      if (i == MenuPage::menucurpage->menu_row) {
        selState = 1;                  ////SELECTED
        if (X->stateprev) selState++;  ////Pushed
        if (X->state_hold) selState++;
        //  display.fillRoundRect(0, ypos - 2, 127, CharH + 3, 3, SSD1306_INVERSE);  //right to left
        //else
        //  display.fillRoundRect(1, ypos - 1, 125, CharH + 1, 3, SSD1306_INVERSE);  //right to left
      }
      printItem(menu_left + 1, menu_top + ypos + 1, menu_width - 2, CharH + 1, strItem, selState);
    }
  }
  menuDisplay();
}


#endif