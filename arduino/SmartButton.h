#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>

class SmartButton {
  public:
    String name;
    int pin;
    int lastState;
    unsigned long pressedTime;
    unsigned long releasedTime;
    bool isPressing;
    bool isLongDetected;

    
  public:
    SmartButton(String _name, int _pin){
      name = _name;
      pin = _pin;
      lastState = LOW;
      pressedTime = 0;
      releasedTime = 0;
      isPressing = false;
      isLongDetected = false;
    }
};

#endif
