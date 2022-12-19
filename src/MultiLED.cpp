#include "MultiLED.h"

LedChannel::LedChannel(uint8_t ledPin, LedColor ledColor, bool inversedLogic, uint8_t negativeLightCorrection)
{
    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, LOW);
    this->pin = ledPin;
    this->color = ledColor;
    this->useInverseLogic = inversedLogic;
    this->currentValue = 0;
    analogWriteRange(1023);
    this->correctionFactor = (float)(255 - negativeLightCorrection) / (float)255;
}

void LedChannel::SetOn()
{
    SetValue(255);
}

void LedChannel::SetOff()
{
    SetValue(0);
}

void LedChannel::WriteToLED(uint16_t value)
{
    uint16_t scaledValue = value * this->correctionFactor * this->brightness;
    analogWrite(pin, scaledValue);
}

void LedChannel::SetValue(uint8_t value)
{
    WriteToLED(value * analogConversionFactor);
    this->currentValue = value;
}

void LedChannel::SetValueHighRes(uint16_t value)
{
    WriteToLED(value);
    this->currentValue = value / analogConversionFactor;
}

void LedChannel::LimitValue(int *value)
{
    if (value == nullptr)
        return;

    *value = (*value < 0) ? 0 : *value;
    *value = (*value > 255) ? 255 : *value;
}

void LedChannel::LimitValues(int *value1, int *value2, int *value3, int *value4, int *value5, int *value6, int *value7, int *value8)
{
    LimitValue(value1);
    LimitValue(value2);
    LimitValue(value3);
    LimitValue(value4);
    LimitValue(value5);
    LimitValue(value6);
    LimitValue(value7);
    LimitValue(value8);
}

void LedChannel::FadeOn(uint16_t duration, FadeProfile profile, int32_t t)
{
    if (t == -1)
    {
        for (int i = 0; i < duration; i++)
        {
            if (duration > 255)
            {
                SetValueHighRes(CalculateFadeValueHighRes(t, duration, profile));
            }
            else
            {
                SetValue(CalculateFadeValue(t, duration, profile));
            }
            sleep(1);
        }
    }
    else
    {
        if (duration > 255)
        {
            SetValueHighRes(CalculateFadeValueHighRes(t, duration, profile));
        }
        else
        {
            SetValue(CalculateFadeValue(t, duration, profile));
        }
    }
}

void LedChannel::FadeOff(uint16_t duration, FadeProfile profile, int32_t t, uint8_t fromValue)
{
    if (t == -1)
    {
        int value = currentValue;
        for (int i = 0; i < duration; i++)
        {
            if (duration > 255)
            {
                SetValueHighRes(CalculateFadeValueHighRes(t, duration, profile, value << 2, 0));
            }
            else
            {
                SetValue(CalculateFadeValue(t, duration, profile, value, 0));
            }
            sleep(1);
        }
    }
    else
    {
        if (duration > 255)
        {
            SetValueHighRes(CalculateFadeValueHighRes(t, duration, profile, fromValue << 2, 0));
        }
        else
        {
            SetValue(CalculateFadeValue(t, duration, profile, fromValue, 0));
        }
    }
}

void LedChannel::FadeToValue(uint8_t fromValue, uint8_t toValue, uint32_t duration, FadeProfile profile, int32_t t)
{
    if (t == -1)
    {
        for (uint16_t i = 0; i < duration; i++)
        {
            if (duration > 255)
            {
                SetValueHighRes(CalculateFadeValueHighRes(t, duration, profile, fromValue << 2, toValue << 2));
            }
            else
            {
                SetValue(CalculateFadeValue(t, duration, profile, fromValue, toValue));
            }
            sleep(1);
        }
    }
    else
    {
        if (duration > 255)
        {
            SetValueHighRes(CalculateFadeValueHighRes(t, duration, profile, fromValue << 2, toValue << 2));
        }
        else
        {
            SetValue(CalculateFadeValue(t, duration, profile, fromValue, toValue));
        }
    }
}

void LedChannel::Pulse(uint8_t value, uint16_t duration, bool restoreOldColor)
{
    uint8_t oldValue = currentValue;
    duration /= 2;
    for (int i = 0; i <= duration; i++)
    {
        SetValueHighRes(CalculateFadeValueHighRes(i, duration, logistic, 0, value << 2));
        sleep(1);
    }
    for (int i = 0; i <= duration; i++)
    {
        SetValueHighRes(CalculateFadeValueHighRes(i, duration, logistic, value << 2, 0));
        sleep(1);
    }
    if (restoreOldColor)
    {
        SetValue(oldValue);
    }
}

uint8_t LedChannel::CalculateFadeValue(uint32_t t, uint32_t period, FadeProfile profile, uint8_t fromValue, uint8_t toValue)
{
    uint8_t calculatedValue;
    switch (profile)
    {
    case logistic:
        calculatedValue = FadeOn_Func_Logistic(t, period, fromValue, toValue);
        break;

    case linear:
        calculatedValue = FadeOn_Func_Linear(t, period, fromValue, toValue);
        break;

    case exponential:
        calculatedValue = FadeOn_Func_Exponential(t, period, fromValue, toValue);
        break;

    default:
        calculatedValue = 0;
    }
    return calculatedValue;
}

uint16_t LedChannel::CalculateFadeValueHighRes(uint32_t t, uint32_t period, FadeProfile profile, uint16_t fromValue, uint16_t toValue)
{
    uint16_t calculatedValue;
    switch (profile)
    {
    case logistic:
        calculatedValue = FadeOn_Func_Logistic(t, period, fromValue, toValue);
        break;

    case linear:
        calculatedValue = FadeOn_Func_Linear(t, period, fromValue, toValue);
        break;

    case exponential:
        calculatedValue = FadeOn_Func_Exponential(t, period, fromValue, toValue);
        break;

    default:
        calculatedValue = 0;
    }
    if (calculatedValue > 1023)
    {
        return 1023;
    }
    return calculatedValue;
}

uint16_t LedChannel::FadeOn_Func_Logistic(uint32_t t, uint32_t period, uint16_t fromValue, uint16_t toValue)
{
    int fadeDifference = toValue - fromValue;

    if (t >= period)
        return toValue; // kFullBrightness;

    // approximate by linear interpolation.
    // scale t according to period to 0..255
    t = ((t << 8) / period) & 0xff;
    const auto i = (t >> 5); // -> i will be in range 0 .. 7
    const auto y0 = (int)(fadeDifference * FadeTableLogistic[i]);
    const auto y1 = (int)(fadeDifference * FadeTableLogistic[i + 1]);
    const auto x0 = i << 5; // *32

    // y(t) = mt+b, with m = dy/dx = (y1-y0)/32 = (y1-y0) >> 5
    return (((t - x0) * (y1 - y0)) >> 5) + y0 + fromValue;
}

uint16_t LedChannel::FadeOn_Func_Exponential(uint32_t t, uint32_t period, uint16_t fromValue, uint16_t toValue)
{
    int fadeDifference = toValue - fromValue;

    if (t >= period)
        return toValue; // kFullBrightness;

    // approximate by linear interpolation.
    // scale t according to period to 0..255
    t = ((t << 8) / period) & 0xff;
    const auto i = (t >> 5); // -> i will be in range 0 .. 7
    const auto y0 = (int)(fadeDifference * FadeTableExponential[i]);
    const auto y1 = (int)(fadeDifference * FadeTableExponential[i + 1]);
    const auto x0 = i << 5; // *32

    // y(t) = mt+b, with m = dy/dx = (y1-y0)/32 = (y1-y0) >> 5
    return (((t - x0) * (y1 - y0)) >> 5) + y0 + fromValue;
}

uint16_t LedChannel::FadeOn_Func_Linear(uint32_t t, uint32_t period, uint16_t fromValue, uint16_t toValue)
{
    if (t >= period)
        return toValue; // kFullBrightness;

    uint8_t y0;
    uint8_t y1;
    float dy;
    float dx = period;
    float k;

    //calculate Value from linear function:
    y0 = fromValue;
    y1 = toValue;
    dy = y1 - y0;
    k = dy / dx;
    return (uint8_t)(k * t + y0);
}

//MultiLed

void MultiLED::AddChannel(uint8_t channelId, LedChannel *channel)
{
    channels[channelId] = channel;
}

void MultiLED::DeleteChannel(uint8_t channelId)
{
    delete channels[channelId];
}

void MultiLED::SetOn()
{
    for (int i = 0; i < MAX_LEDS; i++)
    {
        if (channels[i])
        {
            channels[i]->SetOn();
        }
    }
    mode = fixColor;
}

void MultiLED::SetOff()
{
    for (int i = 0; i < MAX_LEDS; i++)
    {
        if (channels[i])
        {
            channels[i]->SetOff();
        }
    }
    mode = off;
}

void MultiLED::FadeOn(uint16_t period, FadeProfile profile)
{
    for (uint16_t i = 0; i < period; i++)
    {
        for (int j = 0; j < MAX_LEDS; j++)
        {
            if (channels[j])
            {
                channels[j]->FadeOn(period, profile, i);
            }
        }
        sleep(1);
    }
    mode = fixColor;
}

void MultiLED::FadeOnAsync(uint16_t period, FadeProfile profile)
{
    FadeToValueAsync(Color::White, period, profile);
}

void MultiLED::FadeOff(uint16_t period, FadeProfile profile)
{
    uint8_t currentValues[MAX_LEDS];
    for (int i = 0; i < MAX_LEDS; i++)
    {
        if (channels[i])
        {
            currentValues[i] = channels[i]->currentValue;
        }
    }
    for (uint16_t i = 0; i <= period; i++)
    {
        for (int j = 0; j < MAX_LEDS; j++)
        {
            if (channels[j])
            {
                channels[j]->FadeOff(period, profile, i, currentValues[j]);
            }
        }
        sleep(1);
    }
    mode = off;
}

void MultiLED::FadeOffAsync(uint16_t period, FadeProfile profile)
{
    FadeToValueAsync(Color::Black, period, profile);
}

void MultiLED::FadeToValue(RGB value, uint16_t period, FadeProfile profile)
{
    int currentValues[MAX_LEDS];
    for (int i = 0; i < MAX_LEDS; i++)
    {
        if (channels[i])
        {
            currentValues[i] = channels[i]->currentValue;
        }
    }
    for (uint16_t i = 0; i < period; i++)
    {
        for (int j = 0; j < MAX_LEDS; j++)
        {
            if (channels[j])
            {
                if (channels[j]->color == red)
                    channels[j]->FadeToValue(currentValues[j], value.r, period, profile, i);

                if (channels[j]->color == green)
                    channels[j]->FadeToValue(currentValues[j], value.g, period, profile, i);

                if (channels[j]->color == blue)
                    channels[j]->FadeToValue(currentValues[j], value.b, period, profile, i);
            }
        }
        sleep(1);
    }
    mode = CheckMode();
}

void MultiLED::FadeToValueAsync(RGB value, uint32_t period, FadeProfile profile)
{
    mode = fade;

    DeleteAllFadeSteps();
    fadeSteps[0] = new FadeStep{period, value, profile};
}

void MultiLED::StartSunriseFadeAsync(uint8_t durationInMinutes)
{
    uint32_t duration = (uint32_t)durationInMinutes * 60000;
    StartColorFade(ColorFades::Sunrise, false);
    ResizeFadeListDuration(fadeSteps, duration);
}

void MultiLED::Pulse(RGB value, uint16_t duration, bool restoreOldValue)
{
    RGB oldValue = GetCurrentValue();
    Mode oldMode = mode;
    FadeToValue(value, duration / 2);
    FadeToValue({0, 0, 0}, duration / 2);
    if (restoreOldValue)
    {
        SetRGBValue(oldValue);
        mode = oldMode;
    }
    else
    {
        SetRGBValue({0, 0, 0});
        mode = off;
    }
}

void MultiLED::SetBrightness(float brightness)
{
    if(brightness > 1 || brightness < 0)
        return;
    for (int i = 0; i < MAX_LEDS; i++)
    {
        if (channels[i])
        {
            channels[i]->brightness = brightness;
        }
    }
}

void MultiLED::SetRed()
{
    SetColorLed((LedColor)red);

    mode = fixColor;
    currentValue = GetCurrentValue();
}

void MultiLED::SetGreen()
{
    SetColorLed((LedColor)green);

    mode = fixColor;
    currentValue = GetCurrentValue();
}

void MultiLED::SetBlue()
{
    SetColorLed((LedColor)blue);

    mode = fixColor;
    currentValue = GetCurrentValue();
}

void MultiLED::SetWhite()
{
    SetColorLed((LedColor)white1);

    mode = fixColor;
    currentValue = GetCurrentValue();
}

void MultiLED::SetRGBWhite()
{
    SetColorLed((LedColor)red, false, false);
    SetColorLed((LedColor)green, false, false);
    SetColorLed((LedColor)blue, false, false);

    mode = fixColor;
    currentValue = GetCurrentValue();
}

void MultiLED::SetRGBValue(RGB value)
{

    for (int j = 0; j < MAX_LEDS; j++)
    {
        if (channels[j])
        {
            if (channels[j]->color == red)
                channels[j]->SetValue(value.r);

            if (channels[j]->color == green)
                channels[j]->SetValue(value.g);

            if (channels[j]->color == blue)
                channels[j]->SetValue(value.b);
        }
    }
    currentValue = value;
    mode = CheckMode();
}

void MultiLED::SetColorTemperature(uint16_t kelvin)
{
    RGB value = KelvinRGB::KelvinToRGB(kelvin);
    SetRGBValue(value);
    mode = fixColor;
    currentValue = value;
}

void MultiLED::SetColorLed(LedColor color, bool fade, bool singleColorMode)
{
    for (int i = 0; i < MAX_LEDS; i++)
    {
        if (channels[i])
        {
            if (channels[i]->color == color)
            {
                if (fade)
                {
                    channels[i]->FadeOn(1000);
                }
                else
                {
                    channels[i]->SetOn();
                }
            }
            else if (singleColorMode)
            {
                if (fade)
                {
                    channels[i]->FadeOff(1000);
                }
                else
                {
                    channels[i]->SetOff();
                }
            }
        }
    }
}

void MultiLED::handle()
{
    if (mode == fade)
    {
        if (fadeSteps[fadeStepIndex] && fadeStepIndex < MAX_FADESTEPS)
        {
            if (!fadeSteps[fadeStepIndex]->fadeStartTime)
            {
                fadeSteps[fadeStepIndex]->fadeStartTime = millis();
                if (fadeSteps[fadeStepIndex - 1])
                {
                    fadeSteps[fadeStepIndex]->lastValue = fadeSteps[fadeStepIndex - 1]->nextValue;
                }
                else
                {
                    fadeSteps[fadeStepIndex]->lastValue = GetCurrentValue();
                }
            }
            bool fadecomplete = HandleValueToValueFade(fadeSteps[fadeStepIndex]);
            if (fadecomplete)
            {
                fadeSteps[fadeStepIndex]->fadeStartTime = 0;
                fadeStepIndex++;
            }
        }
        else
        {

            if (fadeStepIndex)
            {
                if (fadeSteps[fadeStepIndex - 1])
                {
                    if (fadeSteps[fadeStepIndex - 1]->repeatAfter)
                    {
                        fadeStepIndex = 0;
                    }
                    else
                    {
                        SetRGBValue(fadeSteps[fadeStepIndex - 1]->nextValue);
                        fadeStepIndex = 0;
                        mode = CheckMode();
                    }
                }
                else
                {
                    fadeStepIndex = 0;
                    mode = CheckMode();
                }
            }
            else
            {
                mode = CheckMode();
            }
        }
    }
    yield();
    //HandleValueToValueFade();
}

void MultiLED::updatestate()
{
    Mode tempmode = mode;
    RGB currentValue = GetCurrentValue();
    SetRGBValue(currentValue);
    mode = tempmode;
}

//returns true if FadeStep is complete
bool MultiLED::HandleValueToValueFade(FadeStep *step)
{
    if (mode == fade && step)
    {
        unsigned long currentTime = millis();
        if (currentTime >= (step->fadeStartTime + step->fadeDuration))
        {
            //SetRGBValue(step->nextValue); leads to flicker
            //mode = fade;
            currentValue = step->nextValue;
            return true;
        }
        else
        {
            uint32_t i = currentTime - step->fadeStartTime;
            for (int j = 0; j < MAX_LEDS; j++)
            {
                if (channels[j])
                {
                    if (channels[j]->color == red)
                        channels[j]->FadeToValue(step->lastValue.r, step->nextValue.r, step->fadeDuration, step->fadeProfile, i);

                    if (channels[j]->color == green)
                        channels[j]->FadeToValue(step->lastValue.g, step->nextValue.g, step->fadeDuration, step->fadeProfile, i);

                    if (channels[j]->color == blue)
                        channels[j]->FadeToValue(step->lastValue.b, step->nextValue.b, step->fadeDuration, step->fadeProfile, i);
                }
            }
            currentValue = GetCurrentValue();
            return false;
        }
    }
    return false;
}

RGB MultiLED::GetCurrentValue()
{
    RGB value = {0, 0, 0};
    for (int i = 0; i < MAX_LEDS; i++)
    {
        if (channels[i])
        {
            switch (channels[i]->color)
            {
            case red:
                value.r = channels[i]->currentValue;
                break;

            case green:
                value.g = channels[i]->currentValue;
                break;

            case blue:
                value.b = channels[i]->currentValue;
                break;
            }
        }
    }

    return value;
}

LedSyncInfo MultiLED::GetSyncInfo()
{
    LedSyncInfo info;
    info.Info_mode = mode;
    info.Info_fadeStepIndex = fadeStepIndex;
    for (size_t i = 0; i < MAX_FADESTEPS; i++)
    {
        if (fadeSteps[i])
        {
            info.Info_fadeSteps[i] = FadeStep{fadeSteps[i]->fadeDuration, fadeSteps[i]->nextValue, fadeSteps[i]->fadeProfile, fadeSteps[i]->lastValue, fadeSteps[i]->fadeStartTime, fadeSteps[i]->repeatAfter};
        }
        else
        {
            info.Info_fadeSteps[i] = {};
        }
    }
    info.Info_currentValue = GetCurrentValue();
    info.Info_syncMillis = millis();
    return info;
}

//Writes the serialized SyncInfo into a char buffer and returns how many bytes the buffer contains
size_t MultiLED::GetSerializedSyncInfo(char *buf)
{
    LedSyncInfo info = GetSyncInfo();
    char *serializedInfo = reinterpret_cast<char *>(&info);
    const char *prefix = "SYNCINFO";
    int prefixLength = strlen(prefix);
    int infoLength = sizeof(LedSyncInfo);
    memcpy(buf, prefix, prefixLength);
    memcpy(buf + prefixLength, serializedInfo, infoLength);
    size_t bufSize = prefixLength + infoLength;
    return bufSize;
}

void MultiLED::SyncLedWithInfo(LedSyncInfo info)
{

    fadeStepIndex = info.Info_fadeStepIndex;
    int32_t timeDiff = millis() - info.Info_syncMillis;
    for (int i = 0; i < MAX_FADESTEPS; i++)
    {
        DeleteFadeStep(i);
        if (info.Info_fadeSteps[i].fadeDuration)
        {
            if (info.Info_fadeSteps[i].fadeStartTime)
            {
                fadeSteps[i] = new FadeStep{info.Info_fadeSteps[i].fadeDuration, info.Info_fadeSteps[i].nextValue, info.Info_fadeSteps[i].fadeProfile, info.Info_fadeSteps[i].lastValue, (info.Info_fadeSteps[i].fadeStartTime + timeDiff), info.Info_fadeSteps[i].repeatAfter};
            }
            else
            {
                fadeSteps[i] = new FadeStep{info.Info_fadeSteps[i].fadeDuration, info.Info_fadeSteps[i].nextValue, info.Info_fadeSteps[i].fadeProfile, info.Info_fadeSteps[i].lastValue, info.Info_fadeSteps[i].fadeStartTime, info.Info_fadeSteps[i].repeatAfter};
            }
        }
    }
    SetRGBValue(info.Info_currentValue);
    mode = info.Info_mode;
}

void MultiLED::SyncLedWithSerializedSyncInfo(char *buf)
{
    const char *prefix = "SYNCINFO";
    int prefixLength = strlen(prefix);
    LedSyncInfo *info = reinterpret_cast<LedSyncInfo *>(buf + prefixLength);
    SyncLedWithInfo(*info);
}

void MultiLED::DeleteFadeStep(uint8_t index)
{
    if(fadeSteps[index])
        delete fadeSteps[index];

    fadeSteps[index] = nullptr;
}

void MultiLED::DeleteAllFadeSteps()
{
    for (uint8_t i = 0; i < MAX_FADESTEPS; i++)
    {
        DeleteFadeStep(i);
    }
}

Mode MultiLED::CheckMode()
{
    Mode currentMode;
    if (currentValue.r || currentValue.g || currentValue.b)
    {
        currentMode = fixColor;
    }
    else
    {
        currentMode = off;
    }
    return currentMode;
}

void sleep(unsigned long duration)
{
    unsigned long startmillis = millis();
    while (millis() < (startmillis + duration))
    {
        yield();
    }
}

constexpr KelvinRGB::ColorTemperature KelvinRGB::WhitePoint;
constexpr KelvinRGB::ColorTemperature KelvinRGB::KelvinTable[];

RGB KelvinRGB::KelvinToRGB(uint16_t kelvin)
{
    if (kelvin <= 1000)
        return (RGB){KelvinRGB::KelvinTable[0].red, KelvinRGB::KelvinTable[0].green, KelvinRGB::KelvinTable[0].blue};

    if (kelvin == 6600)
        return (RGB){KelvinRGB::WhitePoint.red, KelvinRGB::WhitePoint.green, KelvinRGB::WhitePoint.blue};

    if (kelvin >= 40000)
        return (RGB){KelvinRGB::KelvinTable[14].red, KelvinRGB::KelvinTable[14].green, KelvinRGB::KelvinTable[14].blue};

    uint8_t index0 = 0;
    uint8_t index1 = 1;

    for (uint8_t i = 0; i <= 14; i++)
    {
        if (kelvin < KelvinRGB::KelvinTable[i].kelvin)
        {
            index0 = i - 1;
            index1 = i;
            break;
        }
    }

    uint8_t y0;
    uint8_t y1;
    float dy;
    uint16_t x0 = KelvinRGB::KelvinTable[index0].kelvin;
    uint16_t x1 = KelvinRGB::KelvinTable[index1].kelvin;
    float dx = x1 - x0;
    uint16_t t = kelvin - x0;
    float k;

    //red:
    y0 = KelvinRGB::KelvinTable[index0].red;
    y1 = KelvinRGB::KelvinTable[index1].red;
    dy = y1 - y0;
    k = dy / dx;
    uint8_t r = (uint8_t)(k * t + y0);

    //green:
    y0 = KelvinRGB::KelvinTable[index0].green;
    y1 = KelvinRGB::KelvinTable[index1].green;
    dy = y1 - y0;
    k = dy / dx;
    uint8_t g = (uint8_t)(k * t + y0);

    //blue:
    y0 = KelvinRGB::KelvinTable[index0].blue;
    y1 = KelvinRGB::KelvinTable[index1].blue;
    dy = y1 - y0;
    k = dy / dx;
    uint8_t b = (uint8_t)(k * t + y0);

    return (RGB){r, g, b};
}