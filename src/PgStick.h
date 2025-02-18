/*
This file is  manage inputPins as button 

#include "PgStick.h"

アナログ入力を中点をOFFとした両側2点入力として使う
*/

#ifndef PgSTICK_H
#define PgSTICK_H

#include <Arduino.h>

//state sw
//#define clicktime 500
//#define dblclicktime 500  //次の操作までの時間

class PgStick {

  /*
PIN_state ______            ____________                  ____________________                     ________
                ____________            __________________                    _____________________
              push        release     push              release             push                 release
                         click!                     doubleclick!
state                      1                             1->2                 

*/

private:
  uint8_t dev_;

  int state = 0;  //
  bool state_click = false;
  bool state_dblclick = false;
  
  unsigned long pushTime = 0;
  unsigned long releaseTime = 0;
  
  bool state_holdup = false;
public:
    bool PushP;
  bool PushM;  

bool state_hold = false;
  int clicktime = 300;
  int dblclicktime = 100;
  int holdtime = 700;
  bool dir=false; //Mside true;
  bool stateprev = false;

  PgStick(const uint8_t cs)
    : dev_(cs) { //should be analogpin
  }
  int Click() {
    return state_click ;
  }
  bool DoubleClick() {
    return state_dblclick;
  }
  bool Hold() {
    return state_holdup;
  }
  int update() {
      int heartbeat = 0;
    state_click = state_dblclick = false;
    state_holdup = false;
    unsigned long now = millis();

    int val = analogRead(dev_) - 512;  //
    if (!PushP)
      PushP = (val > 300);
    else {
      PushP = (val > 200);
    }
    if (!PushM)
      PushM = (val < -300);
    else {
      PushM = (val < -200);
    }

    if ((PushP || PushM) != stateprev) {
        heartbeat = 1;
      if ((PushP || PushM)) {//push
        if (state == 0) {
          pushTime = now;
          dir=PushP;
        }
      } else {//release
        releaseTime = now;
        if (state == 0 && now - pushTime < clicktime) {
          //Click
          state_click = true;
        } else if (state == 1 && now - pushTime < dblclicktime) {
          //dblClick
          state_dblclick = true;
          
        }
        state++;
      }
      state_hold = false;
    } else {
      if (state == 0 && (PushP || PushM) && now - pushTime > holdtime) {
        //Hold
        if (!state_hold) {
          state_holdup = true;  //一回のみ出力//一回限り
        }
        state_hold = true;
      } else if (now - pushTime > dblclicktime) {
        state = 0;
      }
    }
    stateprev = PushP || PushM;

    return (int)stateprev;

  }
};

#endif
