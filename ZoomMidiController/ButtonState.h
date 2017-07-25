#ifndef ButtonState_H
#define ButtonState_H

// Put the entire contents of your header here...
#include "Arduino.h"

class ButtonState
{
  public:
    ButtonState(int buttonPin, int ledPin, int msgIndex, byte* currentPatch, bool isLedAnalog);
    void checkStatus();
    void setLed();
    bool isSingleClick();
    bool isHold();
    void ledFlashOn();
    void ledFlashOff();
    void setButtonOn(bool buttonTrue);
    bool ButtonState::isEmptyEffect(byte* effectPtr);
    
  private:
    enum clickState { reset, singleClick, hold };
    enum ledState {off, on, flash };
    int _buttonPin;
    int _ledPin;
    long _clickTimer = 0;
    long _clickTimerStart = 0;
    long _clickTimerEnd = 0;
    long _ledBlinkStart = 0;
    long _betweenClick;
    int _lastReading;
    int _onOffIndex;
    byte * _currentPatch;
    bool isLedAnalog;

    clickState buttonClickState;
    ledState led;
    bool _hold = false;
    bool _singleClick = false;
    bool _holdOn = false;
    bool _canClick = false;
    bool _canHold = false;
    static const int holdTreshHold  = 2000;
    static const int singleClickTreshHold = 10;
    static const int ledOnTreshHold = 50;
    int onOffIndex = 0;
    long ledFlashTime; 
};
#endif

