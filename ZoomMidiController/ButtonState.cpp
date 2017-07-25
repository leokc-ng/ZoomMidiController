#include "Arduino.h"
#include "ButtonState.h"


ButtonState::ButtonState(int buttonPin, int ledPin, int msgIndex, byte* currentPatch, bool isLedAnalog)
{
  _buttonPin = buttonPin;
  _ledPin = ledPin;
  pinMode(buttonPin, INPUT);
  pinMode(_ledPin, OUTPUT);
  buttonClickState = reset;
  _onOffIndex = msgIndex;
  _currentPatch = currentPatch;
  //Serial.begin(9600);
}

void ButtonState::checkStatus()
{
  int reading = digitalRead(_buttonPin);
  long currentTime = millis();
  if (reading == LOW)
  {
    if (buttonClickState == singleClick)
    {
      //Serial.println("Check Status singleClick");
      _singleClick = true;
      _hold = false;
    }
    else if (buttonClickState == hold)
    {
      //Serial.println("Check Status hold");
      _singleClick = false;
      _hold = true;
    }

    buttonClickState = reset;

    //button is now released
  }
  else
  {
    if (_lastReading == LOW)
    {
      if (led == flash)
      {
        _clickTimerStart = _clickTimerEnd;
        _clickTimerEnd = currentTime;
        ledFlashTime = _clickTimerEnd - _clickTimerStart;
      }
      buttonClickState = reset;
      _clickTimer = currentTime;
    }
    else
    {

      if (currentTime > (_clickTimer + holdTreshHold))
      {
        buttonClickState = hold ;
        //Serial.println("HOLD");
      }
      else if (currentTime > (_clickTimer + singleClickTreshHold))
      {
        buttonClickState = singleClick;
        //Serial.println("Single Click");
      }
    }
  }


  _lastReading = reading;

}

bool ButtonState::isSingleClick()
{

  bool isSingleClick = _singleClick;

  if (_singleClick)
  {
    _singleClick = false;
    _hold = false;
  }

  if (isSingleClick)
  {
    byte effectByte  = _currentPatch[_onOffIndex];
    //Serial.print("Patch value ");
    //Serial.println(effectByte);

    if (isEmptyEffect(&_currentPatch[_onOffIndex]))
    {
      isSingleClick = false;
      //Don't do anything
    }
    else if (effectByte % 16 == 0)
    {
      //Serial.println("Effect off");
      _currentPatch[_onOffIndex] = effectByte + 1;
      //_currentPatch[_onOffIndex] = 0x41;
      //Serial.println("Set effect to On");
    }
    else if (effectByte % 16 == 1)
    {
      // Serial.println("Effect on");
      _currentPatch[_onOffIndex] = effectByte - 1;
      //_currentPatch[_onOffIndex] = 0x40;
      //Serial.println("Set effect to Off");
    }


  }

  return isSingleClick;
}

bool ButtonState::isHold()
{

  bool isHold  = _hold;

  if (_hold)
  {
    //Serial.println("isHOld()");
    _singleClick = false;
    _hold = false;
  }
  return isHold;
}

void ButtonState::setLed()
{

  if (isEmptyEffect(&_currentPatch[_onOffIndex]))
  {
    led = off;  
  }
  else if(_currentPatch[_onOffIndex] % 16 == 1)
  {
    //Serial.println("Set led on");
    led = on;
  }
  else
  {
    //Serial.println("Set led off");
    led = off;
  }

  if (led == off)
  {
    //Serial.println("led  == off");
    if (isLedAnalog)
    {
      analogWrite(_ledPin, 0);

    }
    else
    {
      digitalWrite(_ledPin, LOW);
    }
    //ButtonState::ledFlashOff();
  }
  else if ( led == on)
  {
    //Serial.println("led  == on");
    if (isLedAnalog)
    {
      analogWrite(_ledPin, 255);

    }
    else
    {
      digitalWrite(_ledPin, HIGH);
    }
  }
  else
  {
    //Serial.println("flash");
    Serial.println(ledFlashTime);
    ButtonState::ledFlashOn();
  }

}

void ButtonState::ledFlashOn()
{

  long currentTime = millis();
  if (_ledBlinkStart == 0)
  {
    _ledBlinkStart = currentTime;
  }

  if (ledFlashTime == 0)
  {
    ledFlashTime = 1000;
  }
  if (currentTime < (_ledBlinkStart + ledOnTreshHold))
  {
    digitalWrite(_ledPin, HIGH);
  }
  else if (currentTime < (_ledBlinkStart + ledFlashTime) )
  {
    digitalWrite(_ledPin, LOW);
  }
  else
  {
    _ledBlinkStart = currentTime;
  }
}

void ButtonState::ledFlashOff()
{
  _ledBlinkStart = 0;
}

void ButtonState::setButtonOn(bool buttonTrue)
{
  if (buttonTrue)
  {
    led = on;
  }
  else
  {
    led = off;
  }

}


bool ButtonState::isEmptyEffect(byte* effectPtr)
{

  return (effectPtr[0] == 0x01 &&
          effectPtr[1] == 0x00);// &&
          //effectPtr[2] == 0x00 &&
         // effectPtr[3] == 0x00);
  
  
}



