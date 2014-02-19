#include "Engine_tcod.h"
#include "TCODTools.h"
#include "enums.h"
#include <map>
#include "SoilGrid.h"
#include "Weather.h"
#include "VisualProperties.h"
#include "MapScreen.h"
#include "SideBar.h"
#include "utility.h"

Engine CursesEngine;

const int width = 160;
const int height = 160;

const int screenwidth = 100;
const int screenheight = 48;

// The dimensions of the visible portion of the map.
const int viswidth = 80;
const int visheight = 40;


TCODConsole* worldmap;
ALMANAC::SoilGrid* sg;
ALMANAC::Weather* WeatherModule;
HerbSim::MapScreen* ms;
HerbSim::SideBar* sidebar;

bool Engine::EngineInit()
{
    sg = new ALMANAC::SoilGrid(width, height);
    WeatherModule = new ALMANAC::Weather(true);
    ms = new HerbSim::MapScreen(sg, 81, 41, 0, screenheight - 40 - 1);
    sidebar = new HerbSim::SideBar(19, 41, MabinogiBrown);
    sidebar->rootX = 81;
    sidebar->rootY = 7;
    return true;
}


void Engine::Update()
{

}

void Engine::Render(TCODConsole *root)
{
    ms->Render(root);
    sidebar->Render(root);
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
    ms->KeyDown(key, unicode);
}

void Engine::MouseMoved(const int &iButton, const int &iX, const int &iY, const int &iRelX, const int &iRelY)
{

}

void Engine::MouseButtonUp(const int &iButton, const int &iX, const int &iY, const int &iRelX, const int &iRelY)
{

}

void Engine::MouseButtonDown(const int &iButton, const int &iX, const int &iY, const int &iRelX, const int &iRelY)
{
    coord screencoords(iX/12, iY/12);
    coord relativecoords = ms->RelativeFromScreenCoords(screencoords);
    coord absolutecoords = ms->AbsoluteFromRelativeCoords(relativecoords);

    printf("S(%d,%d)\tR(%d,%d)\tA(%d,%d)\n", screencoords.first, screencoords.second, relativecoords.first, relativecoords.second, absolutecoords.first, absolutecoords.second);
    auto messages = ms->getListing(screencoords);
    if (messages.size() != 0)
        sidebar->setMessages(messages);
}
