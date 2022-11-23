#ifndef H_MULTIBUTTON
#define H_MULTIBUTTON

#include <Arduino.h>

#define LONG_PRESS_TIME 3000

typedef std::function<void()> onPress_t; ///Event notifier callback


class MultiPurposeButton
{
protected:
    uint8_t buttonPin;
    bool lastbuttonState = LOW;
    long unsigned int pressMillis;
    long unsigned int nextPressMillis;
    bool pressed;
    onPress_t shortPressEvent;
    onPress_t longPressEvent;
    

public:

    MultiPurposeButton(uint8_t pinNumber);

    void RegisterCallbacks(onPress_t shortPress, onPress_t longPress = NULL);

    void RegisterShortPressCallback(onPress_t shortPress);

    void RegisterLongPressCallback(onPress_t longPress);

    void handle();

    int GetButtonState();

private:

    void Callback(std::function<void()> event);
};

#endif