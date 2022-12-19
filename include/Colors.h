#ifndef H_COLORS
#define H_COLORS

#include <stdint.h>

typedef struct
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
} RGB;

//Preset Colors
//ColorCodes from Website: https://americanpartylights.com/rgb/
template <class Colors>
struct Color_
{
    static const RGB Black;
    static const RGB Red;
    static const RGB Green;
    static const RGB Blue;
    static const RGB White;
    static const RGB Yellow;
    static const RGB Cyan;
    static const RGB Magenta;
    static const RGB MidnightBlue;
    static const RGB Navy;
    static const RGB CornflowerBlue;
    static const RGB SlateBlue;
    static const RGB MediumBlue;
    static const RGB RoyalBlue;
    static const RGB DodgerBlue;
    static const RGB DeepSkyBlue;
    static const RGB SkyBlue;
    static const RGB SteelBlue;
    static const RGB LightBlue;
    static const RGB Turquoise;
    static const RGB LightCyan;
    static const RGB CadetBlue;

    static const RGB Aquamarine;
    static const RGB DarkGreen;
    static const RGB DarkOliveGreen;
    static const RGB SeaGreen;
    static const RGB PaleGreen;
    static const RGB SpringGreen;
    static const RGB LawnGreen;
    static const RGB GreenYellow;
    static const RGB LimeGreen;
    static const RGB ForestGreen;
    static const RGB OliveDrab;
    static const RGB Khaki;

    static const RGB LightYellow;
    static const RGB Gold;
    static const RGB LightGoldenrod;
    static const RGB Goldenrod;

    static const RGB IndianRed;
    static const RGB SaddleBrown;
    static const RGB Sienna;
    static const RGB SandyBrown;
    static const RGB Tan;
    static const RGB Chocolate;
    static const RGB Brown;

    static const RGB Salmon;
    static const RGB Orange;
    static const RGB DarkOrange;
    static const RGB Coral;
    static const RGB OrangeRed;

    static const RGB HotPink;
    static const RGB DeepPink;
    static const RGB Pink;
    static const RGB Maroon;
    static const RGB VioletRed;
    static const RGB Violet;
    static const RGB DarkViolet;
    static const RGB Purple;
};

//Standard Colors
template <class Colors>
const RGB Color_<Colors>::Black = {0, 0, 0};
template <class Colors>
const RGB Color_<Colors>::Red = {255, 0, 0};
template <class Colors>
const RGB Color_<Colors>::Green = {0, 255, 0};
template <class Colors>
const RGB Color_<Colors>::Blue = {0, 0, 255};
template <class Colors>
const RGB Color_<Colors>::White = {255, 255, 255};
template <class Colors>
const RGB Color_<Colors>::Yellow = {255, 255, 0};
template <class Colors>
const RGB Color_<Colors>::Cyan = {0, 255, 255};
template <class Colors>
const RGB Color_<Colors>::Magenta = {255, 0, 255};

//Blue Colors
template <class Colors>
const RGB Color_<Colors>::MidnightBlue = {25, 25, 112};
template <class Colors>
const RGB Color_<Colors>::Navy = {0, 0, 128};
template <class Colors>
const RGB Color_<Colors>::CornflowerBlue = {100, 149, 237};
template <class Colors>
const RGB Color_<Colors>::SlateBlue = {106, 90, 205};
template <class Colors>
const RGB Color_<Colors>::MediumBlue = {0, 0, 205};
template <class Colors>
const RGB Color_<Colors>::RoyalBlue = {65, 105, 225};
template <class Colors>
const RGB Color_<Colors>::DodgerBlue = {30, 144, 255};
template <class Colors>
const RGB Color_<Colors>::DeepSkyBlue = {0, 191, 255};
template <class Colors>
const RGB Color_<Colors>::SkyBlue = {135, 206, 250};
template <class Colors>
const RGB Color_<Colors>::SteelBlue = {70, 130, 180};
template <class Colors>
const RGB Color_<Colors>::LightBlue = {173, 216, 230};
template <class Colors>
const RGB Color_<Colors>::Turquoise = {64, 224, 208};
template <class Colors>
const RGB Color_<Colors>::LightCyan = {224, 255, 255};
template <class Colors>
const RGB Color_<Colors>::CadetBlue = {95, 158, 160};

//Green Colors
template <class Colors>
const RGB Color_<Colors>::Aquamarine = {127, 255, 212};
template <class Colors>
const RGB Color_<Colors>::DarkGreen = {0, 100, 0};
template <class Colors>
const RGB Color_<Colors>::DarkOliveGreen = {85, 107, 47};
template <class Colors>
const RGB Color_<Colors>::SeaGreen = {46, 139, 87};
template <class Colors>
const RGB Color_<Colors>::PaleGreen = {152, 251, 152};
template <class Colors>
const RGB Color_<Colors>::SpringGreen = {0, 255, 127};
template <class Colors>
const RGB Color_<Colors>::LawnGreen = {124, 252, 0};
template <class Colors>
const RGB Color_<Colors>::GreenYellow = {173, 255, 47};
template <class Colors>
const RGB Color_<Colors>::LimeGreen = {50, 205, 50};
template <class Colors>
const RGB Color_<Colors>::ForestGreen = {34, 139, 34};
template <class Colors>
const RGB Color_<Colors>::OliveDrab = {107, 142, 35};
template <class Colors>
const RGB Color_<Colors>::Khaki = {240, 230, 140};

//Yellow Colors
template <class Colors>
const RGB Color_<Colors>::LightYellow = {255, 255, 224};
template <class Colors>
const RGB Color_<Colors>::Gold = {255, 215, 0};
template <class Colors>
const RGB Color_<Colors>::LightGoldenrod = {238, 221, 130};
template <class Colors>
const RGB Color_<Colors>::Goldenrod = {218, 165, 32};

//Brown Colors
template <class Colors>
const RGB Color_<Colors>::IndianRed = {205, 92, 92};
template <class Colors>
const RGB Color_<Colors>::SaddleBrown = {139, 69, 19};
template <class Colors>
const RGB Color_<Colors>::Sienna = {160, 82, 45};
template <class Colors>
const RGB Color_<Colors>::SandyBrown = {244, 164, 96};
template <class Colors>
const RGB Color_<Colors>::Tan = {210, 180, 140};
template <class Colors>
const RGB Color_<Colors>::Chocolate = {210, 105, 30};
template <class Colors>
const RGB Color_<Colors>::Brown = {165, 42, 42};

//Orange Colors
template <class Colors>
const RGB Color_<Colors>::Salmon = {250, 128, 114};
template <class Colors>
const RGB Color_<Colors>::Orange = {255, 165, 0};
template <class Colors>
const RGB Color_<Colors>::DarkOrange = {255, 140, 0};
template <class Colors>
const RGB Color_<Colors>::Coral = {255, 127, 80};
template <class Colors>
const RGB Color_<Colors>::OrangeRed = {255, 69, 0};

//Pink/Violet Colors
template <class Colors>
const RGB Color_<Colors>::HotPink = {255, 105, 180};
template <class Colors>
const RGB Color_<Colors>::DeepPink = {255, 20, 147};
template <class Colors>
const RGB Color_<Colors>::Pink = {255, 192, 203};
template <class Colors>
const RGB Color_<Colors>::Maroon = {176, 48, 96};
template <class Colors>
const RGB Color_<Colors>::VioletRed = {208, 32, 144};
template <class Colors>
const RGB Color_<Colors>::Violet = {238, 130, 238};
template <class Colors>
const RGB Color_<Colors>::DarkViolet = {148, 0, 211};
template <class Colors>
const RGB Color_<Colors>::Purple = {160, 32, 240};


using Color = Color_<void>; // Allows you to write `Kath::hi`.

template <class ColorThemes>
struct ColorThemes_
{
    static RGB Forest[];
    static RGB Water[];
    static RGB Fire[];
    static RGB Rainbow[];
    static RGB ColorCircle[];
};

template <class ColorThemes>
RGB ColorThemes_<ColorThemes>::Forest[] =
    {
        Color::DarkGreen,
        Color::DarkOliveGreen,
        Color::SeaGreen,
        Color::SpringGreen,

        Color::LawnGreen,
        Color::ForestGreen,
        Color::DodgerBlue,
        Color::SaddleBrown,

        Color::Brown,
        Color::Khaki
};

template <class ColorThemes>
RGB ColorThemes_<ColorThemes>::Water[] =
    {
        Color::Blue,
        Color::DodgerBlue,
        Color::Cyan,
        Color::Navy,
        Color::SteelBlue,
        Color::SkyBlue,
        Color::DeepSkyBlue,
        Color::LightCyan,
        Color::Purple

};

template <class ColorThemes>
RGB ColorThemes_<ColorThemes>::Fire[] =
    {
        Color::Red,
        Color::OrangeRed,
        Color::Orange,
        Color::Yellow

};

template <class ColorThemes>
RGB ColorThemes_<ColorThemes>::Rainbow[] =
    {
        Color::Red,
        Color::OrangeRed,
        Color::LimeGreen,
        Color::Yellow,
        Color::DeepSkyBlue,
        Color::DodgerBlue,
        Color::Purple
};

template <class ColorThemes>
RGB ColorThemes_<ColorThemes>::ColorCircle[] =
    {
        Color::Red,
        Color::Orange,
        Color::Yellow,
        Color::GreenYellow,
        Color::Green,
        Color::SpringGreen,
        Color::Cyan,
        Color::Aquamarine,
        Color::Blue,
        Color::Violet,
        Color::Magenta,
        Color::VioletRed
};

using ColorThemes = ColorThemes_<void>;

#endif