/*
This file is  manage inputPins as button 

#include "PgBUTTON.h"
*/

#ifndef PgBUTTON_H
#define PgBUTTON_H

#include <Arduino.h>

//state sw
//#define clicktime 300
//#define dblclicktime 600    //次の操作までの時間

class PgButton {

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
  bool state_hold = false;
  unsigned long pushTime = 0;
  unsigned long releaseTime = 0;
  bool stateprev = false;
  bool state_holdup = false;

    const int clicktime = 300;
  const int dblclicktime = 600;
public:

  PgButton( const uint8_t cs) : dev_(cs){
    pinMode(dev_, INPUT_PULLUP);
  }
  bool Click(){
    return state_click;
  }
  bool DoubleClick(){
    return state_dblclick;
  }
  bool Hold(){
    return state_holdup;
  }
  void update() {
    state_click = state_dblclick = false;
    state_holdup = false;
    unsigned long now = millis();
    bool Push = (digitalRead(dev_) == 0);
    if (Push != stateprev) {
      if (Push) {
        if (state == 0)  pushTime = now;
      } else {
        releaseTime = now;
        if (state == 0 && now - pushTime < clicktime) {
          //Click
          state_click = true;
        }else if (state == 1 && now - pushTime < dblclicktime) {
          //dblClick
          state_dblclick = true;
        }
        state++;
      }
      state_hold = false;
    } else {
      if (state == 0 && Push && now - pushTime > 3000) {
        //Hold
        if (!state_hold) {
          state_holdup = true;//一回のみ出力//一回限り

        }
        state_hold = true;
      } else if (now - pushTime > dblclicktime) {
        state = 0;
      }
    }
    stateprev = Push;
  }
};

#endif
