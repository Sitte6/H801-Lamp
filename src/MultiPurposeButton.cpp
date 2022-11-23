#include <Arduino.h>
#include "MultiPurposeButton.h"

MultiPurposeButton::MultiPurposeButton(uint8_t pinNumber)
{
  buttonPin = pinNumber;
  pinMode(pinNumber, INPUT);
}

void MultiPurposeButton::RegisterCallbacks(onPress_t shortPress, onPress_t longPress)
{
  shortPressEvent = shortPress;
  longPressEvent = longPress;
}

void MultiPurposeButton::RegisterShortPressCallback(onPress_t shortPress)
{
  shortPressEvent = shortPress;
}

void MultiPurposeButton::RegisterLongPressCallback(onPress_t longPress)
{
  longPressEvent = longPress;
}

void MultiPurposeButton::handle()
{
  
  int currentButtonState = GetButtonState();
  //Serial.println(millis() - pressMillis);
  if (currentButtonState != lastbuttonState)
  {
    if (currentButtonState == LOW)
    {
      pressed = true;
      pressMillis = millis();
    }
    else if(pressed)
    {
      Callback(shortPressEvent);
      pressed = false;
    }
    lastbuttonState = currentButtonState;
  }
  else if (pressed)
  {
    if (currentButtonState == LOW && pressed && (millis() - pressMillis) > LONG_PRESS_TIME)
    {
      pressed = false;
      Callback(longPressEvent);
    }
  }
}

int MultiPurposeButton::GetButtonState()
{
  return digitalRead(buttonPin);
}

void MultiPurposeButton::Callback(std::function<void()> event)
{
  if (event)
  {
    event();
  }
}