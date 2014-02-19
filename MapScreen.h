#pragma once
#include <libtcod.hpp>
#include "soilGrid.h"
#include "utility.h"
#include "VisualProperties.h"
#include <vector>

namespace HerbSim
{
    struct MapScreen
    {
        MapScreen(ALMANAC::SoilGrid* sgp, int screen_width, int screen_height, int blitX = 0, int blitY = 0);
        ~MapScreen();
        
        ALMANAC::SoilGrid* sg;
        int focusX, focusY;
        int screenwidth;
        int screenheight;
        int rootX, rootY;
        int getMapWidth();
        int getMapHeight();
        TCODConsole* console;

        void Render(TCODConsole *root);
        void KeyUp(const int &key, const int &unicode);
        void KeyDown(const int &key, const int &unicode);
        std::vector<ColoredMessage> getListing(coord screenCoords);

        /*
        ScreenCoords : The physical window's coordinates
        RelativeCoords : The Screen's coordinates
        AbsoluteCoords : The SoilGrid's coordinates
        */
        coord RelativeFromScreenCoords(coord screencoords);
        coord AbsoluteFromRelativeCoords(coord relativecoords);
        coord AbsoluteFromScreenCoords(coord screencoords);
    };
}