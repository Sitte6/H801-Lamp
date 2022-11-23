#ifndef H_MULTILED
#define H_MULTILED

#include <random>
#include <Arduino.h>
#include <Colors.h>
#include <ColorFades.h>

#define MAX_LEDS 5
#define MAX_FADESTEPS 10

enum LedColor
{
    red,
    green,
    blue,
    white1,
    white2
};

enum Mode
    {
        off,
        fixColor,
        fade

    };

typedef struct
    {
        uint32_t fadeDuration;
        RGB nextValue;
        FadeProfile fadeProfile;
        RGB lastValue;
        long fadeStartTime;
        bool repeatAfter;

    } FadeStep;

    typedef struct
    {
        Mode Info_mode;
        uint8_t Info_fadeStepIndex;
        FadeStep Info_fadeSteps[MAX_FADESTEPS];
        RGB Info_currentValue;
        long Info_syncMillis;
    } LedSyncInfo;


/*static constexpr uint8_t kFadeOnTable[] = {0, 3, 13, 33, 68,
                                           118, 179, 232, 255};*/

static constexpr float FadeTableLogistic[] = {0, 0.011764F, 0.050980F, 0.129411F, 0.266666F,
                                              0.462745F, 0.701960F, 0.909813F, 1.0F};

static constexpr float FadeTableExponential[] = {0.007843F, 0.007843F, 0.019607F, 0.039215F, 0.078431F,
                                                 0.156862F, 0.274509F, 0.509803F, 1.0F};

void sleep(unsigned long duration);

class KelvinRGB
{
public:
    typedef struct
    {
        uint16_t kelvin;
        uint8_t red;
        uint8_t green;
        uint8_t blue;
    } ColorTemperature;

    static constexpr ColorTemperature WhitePoint = {6600, 255, 249, 253};
    static constexpr ColorTemperature KelvinTable[] =
        {
            {1000, 255, 51, 0},
            {1800, 255, 130, 0},
            {2600, 255, 170, 77},
            {3400, 255, 198, 130},
            {4200, 255, 217, 171},
            {5000, 255, 231, 204},
            {5800, 255, 241, 231},
            {6600, 255, 249, 253}, //WhitePoint
            {7000, 247, 245, 255},
            {8000, 229, 233, 255},
            {9000, 217, 225, 255},
            {10000, 207, 218, 255},
            {20000, 173, 193, 255},
            {30000, 165, 186, 255},
            {40000, 161, 183, 255}};

    static RGB KelvinToRGB(uint16_t kelvin);
};

class LedChannel
{
protected:
    const float analogConversionFactor = 1023/255; 

    void WriteToLED(uint16_t value);

public:
    uint8_t pin;
    LedColor color;
    bool useInverseLogic;
    float correctionFactor;
    uint8_t currentValue;
    LedChannel(uint8_t ledPin, LedColor ledColor, bool inversedLogic = false, uint8_t negativeLightCorrection = 0);

    void SetOn();

    void SetOff();

    void SetValue(uint8_t value);

    void SetValueHighRes(uint16_t value);

    void LimitValue(int *value);

    void LimitValues(int *value1, int *value2, int *value3 = nullptr, int *value4 = nullptr, int *value5 = nullptr, int *value6 = nullptr, int *value7 = nullptr, int *value8 = nullptr);

    void FadeOn(uint16_t duration, FadeProfile profile = logistic, int32_t t = -1);

    void FadeOff(uint16_t duration, FadeProfile profile = logistic, int32_t t = -1, uint8_t fromValue = 255);

    void FadeToValue(uint8_t fromValue, uint8_t toValue, uint32_t duration, FadeProfile profile = logistic, int32_t t = -1);

    void Pulse(uint8_t value, uint16_t duration, bool restoreOldValue = true);

private:
    uint8_t CalculateFadeValue(uint32_t t, uint32_t period, FadeProfile profile = logistic, uint8_t fromValue = 0, uint8_t toValue = 255);
    uint16_t CalculateFadeValueHighRes(uint32_t t, uint32_t period, FadeProfile profile = logistic, uint16_t fromValue = 0, uint16_t toValue = 1023);
    uint16_t FadeOn_Func_Logistic(uint32_t t, uint32_t period, uint16_t fromValue = 0, uint16_t toValue = 255);
    uint16_t FadeOn_Func_Exponential(uint32_t t, uint32_t period, uint16_t fromValue = 0, uint16_t toValue = 255);
    uint16_t FadeOn_Func_Linear(uint32_t t, uint32_t period, uint16_t fromValue = 0, uint16_t toValue = 255);
};

class MultiLED
{
protected:
    //modes:
    //* 0 = led off
    //* 1 = fix color
    //* 2 =
    

    Mode mode;
    uint8_t fadeStepIndex;

    LedChannel *channels[MAX_LEDS];

    

public:
    FadeStep *fadeSteps[MAX_FADESTEPS];
    RGB currentValue;

    void AddChannel(uint8_t channelId, LedChannel *channel);

    void DeleteChannel(uint8_t channelId);

    void SetOn();

    void SetOff();

    void FadeOn(uint16_t period = 1000, FadeProfile profile = logistic);

    void FadeOnAsync(uint16_t period = 1000, FadeProfile profile = logistic);

    void FadeOff(uint16_t period = 1000, FadeProfile profile = logistic);

    void FadeOffAsync(uint16_t period = 1000, FadeProfile profile = logistic);

    void FadeToValue(RGB value, uint16_t period = 1000, FadeProfile profile = logistic);

    void FadeToValueAsync(RGB value, uint32_t period = 1000, FadeProfile profile = logistic);

    template <size_t N>
    void StartColorFade(FadeColor (&fadelist)[N], bool repeating = false)
    {
        mode = fade;
        unsigned short n = (n > MAX_FADESTEPS) ? MAX_FADESTEPS : N;

        DeleteAllFadeSteps();
        for (uint8_t i = 0; i < 5; i++)
        {

            fadeSteps[i] = new FadeStep{fadelist[i].fadeDuration, fadelist[i].nextValue, fadelist[i].fadeProfile};
        }
        if (n > 0 && fadeSteps[5 - 1])
        {
            fadeSteps[n - 1]->repeatAfter = repeating;
        }
    }

    template <size_t N>
    void StartColorFade(RGB (&fadelist)[N], uint32_t durationPerColor = 3000, FadeProfile profile = linear, bool repeating = false)
    {
        mode = fade;

        unsigned short n = (N > MAX_FADESTEPS) ? MAX_FADESTEPS : N;

        DeleteAllFadeSteps();
        for (uint8_t i = 0; i < n; i++)
        {
            fadeSteps[i] = new FadeStep{durationPerColor, fadelist[i], profile};
        }
        if (n > 0 && fadeSteps[n - 1])
        {
            fadeSteps[n - 1]->repeatAfter = repeating;
        }
    }

    template <size_t N>
    void StartRandomColorFadeFromTheme(RGB (&theme)[N], uint32_t durationPerColor = 3000, FadeProfile profile = linear, bool repeating = true)
    {

        mode = fade;

        std::default_random_engine generator(millis());
        std::uniform_int_distribution<std::mt19937::result_type> distN(0, (N - 1));
        for (uint8_t i = 0; i < MAX_FADESTEPS; i++)
        {
            DeleteFadeStep(i);
            fadeSteps[i] = new FadeStep{durationPerColor, theme[distN(generator)], profile};
            sleep(1);
        }
        if (fadeSteps[MAX_FADESTEPS - 1])
        {
            fadeSteps[MAX_FADESTEPS - 1]->repeatAfter = repeating;
        }
    }

    template <size_t N>
    void ResizeFadeListDuration(FadeStep *(&fadelist)[N], uint32_t newDuration)
    {
        uint32_t currentDuration = 0;
        unsigned short n = (N > MAX_FADESTEPS) ? MAX_FADESTEPS : N;
        for (uint8_t i = 0; i < n; i++)
        {
            if (fadelist[i])
            {
                currentDuration += fadelist[i]->fadeDuration;
            }
            else
            {
                break;
            }
        }
        if (currentDuration > 0)
        {
            float resizeFactor = (float)newDuration / (float)currentDuration;
            for (uint8_t i = 0; i < n; i++)
            {
                if (fadelist[i])
                {
                    fadelist[i]->fadeDuration = (uint32_t)(fadelist[i]->fadeDuration * resizeFactor);
                }
                else
                {
                    break;
                }
                //fadelist[0]->fadeDuration = (uint16_t)(fadelist[i]->fadeDuration * resizeFactor);
                //
            }
        }
    }

    void StartSunriseFadeAsync(uint8_t durationInMinutes);

    void Pulse(RGB value, uint16_t duration, bool restoreOldValue = true);

    void SetRed();

    void SetGreen();

    void SetBlue();

    void SetWhite();

    void SetRGBWhite();

    void SetRGBValue(RGB value);

    void SetColorTemperature(uint16_t kelvin);

    void handle();

    //returns true if FadeStep is complete
    bool HandleValueToValueFade(FadeStep *step);

    RGB GetCurrentValue();

    LedSyncInfo GetSyncInfo();

    size_t GetSerializedSyncInfo(char* buf);

    void SyncLedWithInfo(LedSyncInfo info);

    void SyncLedWithSerializedSyncInfo(char *buf);

private:
    void SetColorLed(LedColor color, bool fade = false, bool singleColorMode = true);

    void DeleteFadeStep(uint8_t index);

    void DeleteAllFadeSteps();

    Mode CheckMode();
};

#endif