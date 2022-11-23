#ifndef H_COLORFADES
#define H_COLORFADES

#include "stdint.h"
#include "Colors.h"

enum FadeProfile
{
    linear,
    logistic,
    exponential
};

typedef struct
{
    uint32_t fadeDuration;
    RGB nextValue;
    FadeProfile fadeProfile;
} FadeColor;


template< class ColorFades >
struct ColorFades_
{
    static FadeColor Sunrise[];
};

template< class ColorFades >
FadeColor ColorFades_<ColorFades>::Sunrise[]= 
  {
        {1, Color::Black, linear},
        {400, Color::Red, exponential},
        {200, Color::Yellow, linear},
        {75,  128, 128, 240, linear},
        {1, Color::Black, linear}
  };

using ColorFades = ColorFades_<void>; 

#endif