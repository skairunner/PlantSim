#include "Engine_tcod.h"
#include "TCODTools.h"
#include "enums.h"
#include <map>
#include "SoilGrid.h"
#include "Weather.h"

Engine CursesEngine;

const int width = 160;
const int height = 160;

const int screenwidth = 100;
const int screenheight = 48;

class SoilColorDict
{
public:
    SoilColorDict()  /// MUST BE CREATED __AFTER__ al_primatives IS INSTALLED o_O
    {
        backcolors[stCLAY] = TCODColor(240, 190, 153);
        backcolors[stSANDYCLAY] = TCODColor(220, 217, 198);
        backcolors[stSILTYCLAY] = TCODColor(200, 173, 152);
        backcolors[stCLAYLOAM] = TCODColor(192, 161, 140);
        backcolors[stSILTYCLAYLOAM] = TCODColor(187, 174, 168);
        backcolors[stSANDYCLAYLOAM] = TCODColor(211, 193, 157);
        backcolors[stLOAM] = TCODColor(202, 195, 169);
        backcolors[stSILTLOAM] = TCODColor(227, 226, 214);
        backcolors[stSANDYLOAM] = TCODColor(209, 204, 162);
        backcolors[stLOAMYSAND] = TCODColor(194, 187, 159);
        backcolors[stSAND] = TCODColor(239, 236, 205);
        backcolors[stSILT] = TCODColor(166, 168, 167);

        frontcolors[stgCLAYGROUP] = TCODColor(109, 74, 39);
        frontcolors[stgSILTGROUP] = TCODColor(109, 74, 39);
        frontcolors[stgSANDGROUP] = TCODColor(255, 255, 255);
        frontcolors[stgLOAMGROUP] = TCODColor(109, 74, 39);

        icons[stgCLAYGROUP] = 0;
        icons[stgSILTGROUP] = 176;
        icons[stgSANDGROUP] = 177;
        icons[stgLOAMGROUP] = 176;
    }

    TCODColor getBackColor(const int soilType)
    {
        return backcolors.find(soilType)->second;
    }

    TCODColor getFrontColor(const int soilGroup)
    {
        return frontcolors.find(soilGroup)->second;
    }

    int getIcon(const int soilGroup)
    {
        return icons.find(soilGroup)->second;
    }
private:
    map<int, TCODColor> backcolors; // 12 backcolors
    map<int, TCODColor> frontcolors; // 4 frontcolors
    map<int, int> icons;
};

class SoilNameDict
{
public:
    SoilNameDict()
    {
        names[stCLAY] = "Clay";
        names[stCLAY] = "Clay";
        names[stSANDYCLAY] = "Sandy clay";
        names[stSILTYCLAY] = "Silty clay";
        names[stCLAYLOAM] = "Clay loam";
        names[stSILTYCLAYLOAM] = "Silty clay loam";
        names[stSANDYCLAYLOAM] = "Sandy clay loam";
        names[stLOAM] = "Loam";
        names[stSILTLOAM] = "Silt loam";
        names[stSANDYLOAM] = "Sandy loam";
        names[stLOAMYSAND] = "Loamy sand";
        names[stSAND] = "Sand";
        names[stSILT] = "Silt";
    }

    std::string getSoilName(int soilType)
    {
        return names.find(soilType)->second;
    }

private:
    map<int, std::string> names;

};

SoilColorDict soilcolorDict;
SoilNameDict soilnameDict;

TCODConsole* worldmap;
ALMANAC::SoilGrid* sg;
ALMANAC::Weather* WeatherModule;

bool Engine::EngineInit()
{
    sg = new ALMANAC::SoilGrid(width, height);
    WeatherModule = new ALMANAC::Weather(true);
    worldmap = new TCODConsole(width, height);

    for (int xcounter = 0; xcounter < width; xcounter++)
    for (int ycounter = 0; ycounter < height; ycounter++)
    {
        auto c = sg->ref(xcounter, ycounter);
        int character = soilcolorDict.getIcon(c.getTopsoilGroup());
        TCODColor fore = soilcolorDict.getFrontColor(c.getTopsoilGroup());
        TCODColor back = soilcolorDict.getBackColor(c.getTopsoilType());
        worldmap->putCharEx(xcounter, ycounter, character, fore, back);
    }
    return true;
}


void Engine::Update()
{

}

void Engine::Render(TCODConsole *root)
{
    TCODConsole::blit(worldmap, 0, 0, 0, 0, root, 0, 0, 1.0, 1.0);
}

void Engine::EngineEnd()
{
}

//
void Engine::Resize(int new_w, int new_h)
{
}

void Engine::WindowActive()
{
}

void Engine::WindowInactive()
{
}

void Engine::KeyUp(const int &key, const int &unicode)
{
}

void Engine::KeyDown(const int &key, const int &unicode)
{

}

void Engine::MouseMoved(const int &iButton, const int &iX, const int &iY, const int &iRelX, const int &iRelY)
{

}

void Engine::MouseButtonUp(const int &iButton, const int &iX, const int &iY, const int &iRelX, const int &iRelY)
{

}

void Engine::MouseButtonDown(const int &iButton, const int &iX, const int &iY, const int &iRelX, const int &iRelY)
{

}
