#ifndef FAITHFUL_CONFIG_FONTINFO_H_
#define FAITHFUL_CONFIG_FONTINFO_H_

namespace faithful {
namespace config {
namespace font {

/// Each bitmap contains all these symbols and only them with the same order.
/// Order - ascending ASCII, starting from 32.
/// Last symbol is actually 'not supported' like '[]'
/// for whitespace first is used
inline constexpr char font_chars[] = {
    ' ', '!', '"', '#', '$', '%', '&', '\'', '(', ')', '*', '+',
    ',', '-', '.', '/', '0', '1', '2', '3', '4', '5', '6', '7',
    '8', '9', ':', ';', '<', '=', '>', '?', '@', 'A', 'B', 'C',
    'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
    'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '[',
    '\\', ']', '^', '_', '`', 'a', 'b', 'c', 'd', 'e', 'f', 'g',
    'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's',
    't', 'u', 'v', 'w', 'x', 'y', 'z', '{', '|', '}', '~', ' '
};

/// N x M resolution can be deduced from __cell_height:
/// 64 for 512 and 32 for 256

inline constexpr int bitmap_rows_num = 8;
inline constexpr int bitmap_columns_num = 12;

/// Main Menu Button
/// 512 x 512
inline constexpr int MainMenuButton_YvGothicMedium__cell_height = 64;
inline constexpr int MainMenuButton_YvGothicMedium__cell_width = 40;
inline constexpr int MainMenuButton_YvGothicMedium__font_height = 50;
inline constexpr int MainMenuButton_YvGothicMedium__font_width = 34;
/// in redactor + param Bold (just for clarity)
inline constexpr int MainMenuButton_YvGothicMedium__symbol_width[] = {
    11, 13, 15, 21, 22, 29, 26, 9, 14, 14, 18, 27,
    10, 16, 10, 18, 21, 21, 21, 21, 21, 21, 21, 21,
    21, 21, 11, 11, 27, 27, 27, 20, 31, 24, 24, 25,
    27, 23, 22, 27, 27, 11, 15, 25, 22, 33, 27, 27,
    24, 27, 25, 23, 23, 27, 24, 34, 24, 24, 23, 14,
    21, 14, 19, 19, 19, 21, 21, 19, 21, 21, 13, 20,
    21, 10, 11, 20, 10, 31, 21, 21, 21, 21, 14, 19,
    13, 21, 18, 28, 18, 18, 17, 14, 14, 14, 26, 36
};

/// Main Menu Description
/// 256 x 256
inline constexpr int MainMenuDescription_Cambria__cell_height = 32;
inline constexpr int MainMenuDescription_Cambria__cell_width = 20;
inline constexpr int MainMenuDescription_Cambria__font_height = 30;
inline constexpr int MainMenuDescription_Cambria__font_width = 13;
/// in redactor + param Bold + param Italic (just for clarity)
inline constexpr int MainMenuDescription_Cambria__symbol_width[] = {
    5, 7, 9, 13, 12, 22, 16, 5, 9, 9, 10, 13,
    5, 7, 5, 11, 13, 13, 13, 13, 13, 13, 13, 13,
    13, 13, 6, 6, 13, 13, 13, 10, 20, 14, 14, 12,
    15, 12, 12, 14, 15, 8, 7, 15, 12, 18, 15, 15,
    13, 15, 14, 11, 13, 15, 13, 21, 13, 13, 12, 8,
    11, 8, 13, 8, 6, 13, 13, 10, 13, 11, 7, 13,
    13, 7, 7, 12, 7, 19, 13, 12, 13, 13, 10, 10,
    8, 13, 11, 17, 11, 11, 10, 8, 7, 8, 13, 15
};

/// Main Menu Version
/// 256 x 256
inline constexpr int MainMenuVersion_MalgunGothic__cell_height = 32;
inline constexpr int MainMenuVersion_MalgunGothic__cell_width = 20;
inline constexpr int MainMenuVersion_MalgunGothic__font_height = 24;
inline constexpr int MainMenuVersion_MalgunGothic__font_width = 9;

inline constexpr int MainMenuVersion_MalgunGothic__symbol_width[] = {
    6, 5, 7, 11, 10, 15, 15, 4, 6, 6, 8, 13,
    4, 8, 4, 7, 10, 10, 10, 10, 10, 10, 10, 10,
    10, 10, 4, 4, 13, 13, 13, 8, 18, 12, 11, 12,
    13, 9, 9, 13, 13, 5, 7, 11, 9, 17, 14, 14,
    10, 14, 11, 10, 10, 13, 12, 17, 11, 10, 11, 6,
    14, 6, 13, 8, 5, 10, 11, 9, 11, 10, 6, 11,
    11, 5, 5, 9, 5, 16, 11, 11, 11, 11, 6, 8,
    6, 11, 9, 13, 9, 9, 8, 6, 4, 6, 13, 5
};

/// Game Loot/Mob
/// 256 x 256
inline constexpr int GameItemMob_BahnschriftLightCondensed__cell_height = 32;
inline constexpr int GameItemMob_BahnschriftLightCondensed__cell_width = 20;
inline constexpr int GameItemMob_BahnschriftLightCondensed__font_height = 32;
inline constexpr int GameItemMob_BahnschriftLightCondensed__font_width = 16;

inline constexpr int GameItemMob_BahnschriftLightCondensed__symbol_width[] = {
    6, 6, 8, 13, 12, 17, 14, 4, 6, 6, 8, 9,
    5, 10, 5, 8, 12, 8, 11, 11, 12, 11, 11, 11,
    12, 11, 5, 5, 8, 8, 8, 10, 20, 12, 12, 12,
    12, 11, 10, 12, 12, 5, 10, 12, 10, 16, 13, 13,
    12, 13, 13, 12, 10, 12, 11, 16, 11, 10, 10, 6,
    8, 6, 10, 9, 5, 11, 11, 11, 11, 11, 7, 11,
    12, 6, 6, 11, 6, 18, 12, 11, 11, 11, 9, 10,
    7, 11, 11, 16, 11, 11, 10, 7, 5, 7, 9, 14
};

/// Game Storytelling
/// 512 x 512
inline constexpr int GameStorytelling_Constantia__cell_height = 64;
inline constexpr int GameStorytelling_Constantia__cell_width = 40;
inline constexpr int GameStorytelling_Constantia__font_height = 64;
inline constexpr int GameStorytelling_Constantia__font_width = 20;
/// in redactor + param Bold (just for clarity)
inline constexpr int GameStorytelling_Constantia__symbol_width[] = {
    8, 11, 13, 19, 15, 29, 25, 7, 13, 13, 15, 19,
    10, 12, 10, 14, 20, 13, 17, 16, 18, 16, 19, 16,
    19, 19, 10, 10, 19, 19, 19, 16, 31, 23, 23, 23,
    28, 21, 20, 26, 29, 14, 13, 25, 20, 33, 26, 29,
    22, 29, 25, 18, 22, 27, 23, 34, 24, 22, 21, 12,
    14, 12, 19, 17, 14, 19, 21, 17, 22, 19, 12, 19,
    22, 12, 11, 21, 11, 33, 22, 21, 21, 21, 15, 16,
    14, 22, 18, 27, 19, 18, 18, 13, 12, 13, 19, 17
};

} // font
} // config
} // faithful

#endif  // FAITHFUL_CONFIG_FONTINFO_H_
