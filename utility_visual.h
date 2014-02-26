#pragma once
#include "libtcod.hpp"
#include <map>
#include "enums.h"

class SoilDictionary
{
public:
    SoilDictionary();
    TCODColor getBackColor(const int soilType);
    TCODColor getFrontColor(const int soilGroup);
    int getIcon(const int soilGroup);
    std::string getSoilName(int soilType);
private:
    std::map<int, TCODColor> backcolors; // 12 backcolors
    std::map<int, TCODColor> frontcolors; // 4 frontcolors
    std::map<int, int> icons;
    std::map<int, std::string> names;
};

struct ColoredMessage // A struct of <text, foreground color, background color> to use for passing 
{
    ColoredMessage();
    ColoredMessage(const std::string& content, const TCODColor& forecolor = TCODColor(255, 0, 255), const TCODColor& backcolor = TCODColor(255, 0, 255));
    std::string text;
    TCODColor fore;
    TCODColor back;
};

extern SoilDictionary soilDict;
extern TCODColor MabinogiBrown;
extern TCODColor DarkMabinogiBrown;