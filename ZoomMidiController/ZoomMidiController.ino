#include "ButtonState.h"
//#include <max3421e.h>
#include <usbh_midi.h>
#include <usb.h>

// Satisfy the IDE, which needs to see the include statment in the ino too.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#include <SPI.h>
#endif

/*#ifdef USBCON
  #define _MIDI_SERIAL_PORT Serial1
  #else
  #define _MIDI_SERIAL_PORT Serial
  #endif*/
//////////////////////////
// MIDI Pin assign
// 2 : GND
// 4 : +5V(Vcc) with 220ohm
// 5 : TX
//////////////////////////

USB Usb;

USBH_MIDI  Midi(&Usb);

byte currentPatch[146];
//bool buttonStateChanges = false;
const int butt1Pin = 2;
const int butt1LedPin = 19;
const int butt1EffIdx = 6;
ButtonState button1(butt1Pin, butt1LedPin, butt1EffIdx, currentPatch, false );

const int butt2Pin = 3;
const int butt2LedPin = 18;
const int butt2EffIdx = 26;
ButtonState button2(butt2Pin, butt2LedPin, butt2EffIdx, currentPatch, false );

const int butt3Pin = 4;
const int butt3LedPin = 17;
const int butt3EffIdx = 47;
ButtonState button3(butt3Pin, butt3LedPin, butt3EffIdx, currentPatch, false);

const int butt4Pin = 5;
const int butt4LedPin = 16;
const int butt4EffIdx = 67;
ButtonState button4(butt4Pin, butt4LedPin, butt4EffIdx, currentPatch, false);

const int butt5Pin = 6;
const int butt5LedPin = 15;
const int butt5EffIdx = 88;
ButtonState button5(butt5Pin, butt5LedPin, butt5EffIdx, currentPatch, false );


const int butt6Pin = 7;
const int butt6LedPin = 14;
const int butt6EffIdx = 108;
ButtonState button6(butt6Pin, butt6LedPin, butt6EffIdx, currentPatch, false );
bool deviceReady = false;
bool currentBank = 0;


//long debounceDelay = 5;
//int patchSize;
//bool ledOn;
//byte preset = 1;



void setup()
{


  Serial.begin(38400);
  while (Usb.Init() == -1)
  {
    Serial.println("OSC did not start.");
    delay(200);

  }

  Serial.println("Ready");

}

void loop()
{

  if (!deviceReady )
  {
    Usb.Task();
    uint8_t state = Usb.getUsbTaskState();
    if ( state == USB_STATE_RUNNING )
    {
      //deviceReady = true;
      SetEditModeOn();
      
      getCurrentPatch(true);
      if (currentPatch[4] == 0x28)
      {
        deviceReady = true;
      }
    }
    else
    {
      //Serial.println("Not Ready!");
    }
  }
  else
  {

    byte commandMsg[6];
    //getButtonsClicked();

    if (getButtonsClicked())
    {

      //Serial.println("Button Clicked............");
      //delay(5000);
      //Make sure current patch is in edit mode;
      //SetEditModeOn();
      //send current patch
      //Serial.print("Patch value1 ");
      //Serial.println(currentPatch[6], HEX);
      SendMIDI(currentPatch);
      //get current patch
      getCurrentPatch(true);

    }
    else
    {
      //Continue listen to midi message
      //int mil = millis();
      //bool getUpdate = mil%100 == 0;
      getCurrentPatch(true);
    }

  }

}
// Send "Program Change" MIDI Message

void SetEditModeOn()
{
  byte commandMsg[6];
  generateCommandMessage(0x50, commandMsg);
  SendMIDI(commandMsg);
}

void GetCurrentBankNumber()
{
  byte commandMsg[6];
  generateCommandMessage(0x33, commandMsg);
  SendMIDI(commandMsg);
}

void SendMIDI(byte * midiMessage)
{

  Usb.Task();
  uint8_t state = Usb.getUsbTaskState();
  if ( state == USB_STATE_RUNNING )
  {
    //Serial.println("Usb in Running");
    Midi.SendData(midiMessage);          // Send the message
    delay(10);
  }
  else if (state == USB_STATE_CONFIGURING)
  {
    //Serial.println("state for Configuring")  ;
  }
  else if (state  == USB_DETACHED_SUBSTATE_WAIT_FOR_DEVICE)
  {
    //Serial.println("Wait for device")  ;

  }
  else
  {
    //Serial.println("Else");
    //Serial.println(state);
  }

}

void generateCommandMessage(byte commandByte, byte midiMessage[6])
{

  midiMessage[0] = 0xF0;
  midiMessage[1] = 0x52;
  midiMessage[2] = 0x00;
  midiMessage[3] = 0x61;
  midiMessage[4] = commandByte;
  midiMessage[5] = 0xF7;

}

void getCurrentPatch(bool request)
{

  //Always make sure is in edit mode.
  byte* midiMessage = new byte[6];
  
  if (request)
  {
    generateCommandMessage(0x29, midiMessage);
    SendMIDI(midiMessage);
    //Poll to get the  current patch information.
  }

  if (MIDI_poll())
  {
    button1.setLed();
    button2.setLed();
    button3.setLed();
    button4.setLed();
    button5.setLed();
    button6.setLed();
  }


}




bool MIDI_poll()
{
  uint8_t size;
  byte outBuff[3];
  byte sysexBuff[3];
  byte  replyMsg[400];
  int msgIndex = 0;
  bool patchUpdate = false;
  //Serial.println("Start midi poll");
  do
  {
    if (size = Midi.RecvRawData(outBuff) > 0)
    {
      //Serial.print("Message receive ");
      //Serial.println(size);
      uint8_t rc = Midi.extractSysExData(outBuff, sysexBuff);

      if (rc > 0  )
      {
        memcpy(&replyMsg[msgIndex], sysexBuff, sizeof(uint8_t)*rc);
        msgIndex += rc;
      }

    }


  } while (size > 0);

  if ( msgIndex == 146 && replyMsg[4] == 0x28)
  {
   
    patchUpdate = true;
    //if (msgIndex = 146)
    //{
      memcpy(currentPatch, replyMsg, sizeof(uint8_t)*msgIndex);
    //}
    if (!deviceReady)
    {
      deviceReady = true;

    }

  }
  /*else if (msgIndex >0)
  {
      //printSysexmsg(replyMsg, msgIndex);
  }*/
  /*else
    {
    Serial.print(" Not Patch Message, size ");
    Serial.println(msgIndex);
    Serial.println(replyMsg[4]);
    //delay(2000);

    }*/


  return patchUpdate;
}

bool getButtonsClicked()
{
  //Serial.println("getButtonsClicked");
  //Serial.print("Before: ");
  //Serial.println(currentPatch[butt1EffIdx]);
  button1.checkStatus();
  button2.checkStatus();
  button3.checkStatus();
  button4.checkStatus();
  button5.checkStatus();
  button6.checkStatus();



  bool clicked = button1.isSingleClick() || button2.isSingleClick() ||
                 button3.isSingleClick() || button4.isSingleClick() ||
                 button5.isSingleClick() || button6.isSingleClick();
  return clicked;

}

void printSysexmsg( byte* msg, int size)
{


  for (int i = 0; i < size; i++)
  {

    if (i == 6 || i == 26 || i == 47 ||
        i == 67 || i == 88 || i == 108 ||
        i == 108 || i == 129 || i ==145)
    {
      Serial.println();
    }

    Serial.print(currentPatch[i], HEX);
    Serial.print(" ");


  }
  Serial.println();
}

