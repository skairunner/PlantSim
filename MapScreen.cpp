#include "MapScreen.h"
#include "VisualProperties.h"
#include "SDL.h"
using namespace HerbSim;

MapScreen::MapScreen(ALMANAC::SoilGrid* sgp, int screen_width, int screen_height, int blitX, int blitY)
{
    sg = sgp;
    screenwidth = screen_width;
    screenheight = screen_height;
    rootX = blitX;
    rootY = blitY;
    focusX = screenwidth / 2;
    focusY = screenheight / 2;

    console = new TCODConsole(sg->getWidth(), sg->getWidth());

    for (int xcounter = 0; xcounter < sg->getWidth(); xcounter++)
    for (int ycounter = 0; ycounter < sg->getHeight(); ycounter++)
    {
        auto c = sg->ref(xcounter, ycounter);
        int character = soilDict.getIcon(c.getTopsoilGroup());
        TCODColor fore = soilDict.getFrontColor(c.getTopsoilGroup());
        TCODColor back = soilDict.getBackColor(c.getTopsoilType());
        console->putCharEx(xcounter, ycounter, character, fore, back);
    }
}

MapScreen::~MapScreen()
{
    if (console)
        delete console;
}


void MapScreen::Render(TCODConsole *root)
{
    TCODConsole::blit(console, focusX - screenwidth / 2, focusY - screenheight / 2, screenwidth, screenheight, root, rootX, rootY, 1.0, 1.0);
}

void MapScreen::KeyUp(const int &key, const int &unicode)
{

}

void MapScreen::KeyDown(const int &key, const int &unicode)
{
    switch (key)
    {
    case SDLK_DOWN:
        focusY++;
        if (focusY > sg->getHeight() - screenheight / 2)
            focusY = sg->getHeight() - screenheight / 2;
        break;
    case SDLK_UP:
        focusY--;
        if (focusY < screenheight / 2)
            focusY = screenheight / 2;
        break;
    case SDLK_LEFT:
        focusX--;
        if (focusX < screenwidth / 2)
            focusX = screenwidth / 2;
        break;
    case SDLK_RIGHT:
        focusX++;
        if (focusX > sg->getWidth() - screenwidth / 2)
            focusX = sg->getWidth() - screenwidth / 2;
        break;
    }
}

std::vector<ColoredMessage> MapScreen::getListing(coord screenCoords)
{
    coord abscoord = AbsoluteFromScreenCoords(screenCoords);
    std::vector<ColoredMessage> messages;
    if (sg && &sg->ref(abscoord.first, abscoord.second) != &sg->null) // If sg exists & the cell exists
    {
        auto& cell = sg->ref(abscoord.first, abscoord.second);
        ColoredMessage soil;
        auto soilgroup = cell.getTopsoilGroup();
        auto soiltype = cell.getTopsoilType();
        auto soilname = soilDict.getSoilName(soiltype);
        TCODColor fore = soilDict.getBackColor(soiltype);
        messages.push_back(ColoredMessage(soilname, fore));
    }

    return messages;
}

coord MapScreen::RelativeFromScreenCoords(coord screencoords)
{
    screencoords.first -= rootX;
    screencoords.second -= rootY;
    return screencoords;
}

coord MapScreen::AbsoluteFromRelativeCoords(coord relativecoords)
{
    int XRelativeToFocus = relativecoords.first + focusX - screenwidth/2;
    int YRelativeToFocus = relativecoords.second + focusY - screenheight/2;
    relativecoords.first = XRelativeToFocus;
    relativecoords.second = YRelativeToFocus;
    return relativecoords;
}

coord MapScreen::AbsoluteFromScreenCoords(coord screencoords)
{
    coord relcoords = RelativeFromScreenCoords(screencoords);
    return AbsoluteFromRelativeCoords(relcoords);
}