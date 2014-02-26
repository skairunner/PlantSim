#include "MapScreen.h"
#include "utility_visual.h"
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
    ground = new TCODConsole(sg->getWidth(), sg->getWidth());
    plants = new TCODConsole(sg->getWidth(), sg->getWidth());
    plants->setKeyColor(TCODColor::black);

    for (int xcounter = 0; xcounter < sg->getWidth(); xcounter++)
    for (int ycounter = 0; ycounter < sg->getHeight(); ycounter++)
    {
        auto c = sg->ref(xcounter, ycounter);
        int character = soilDict.getIcon(c.getTopsoilGroup());
        TCODColor fore = soilDict.getFrontColor(c.getTopsoilGroup());
        TCODColor back = soilDict.getBackColor(c.getTopsoilType());
        ground->putCharEx(xcounter, ycounter, character, fore, back);
    }

    for (int xcounter = 0; xcounter < sg->getWidth(); xcounter++)
    for (int ycounter = 0; ycounter < sg->getHeight(); ycounter++)
    {
        int tallestIndex = -1;
        int coverIndex = -1;
        double tallest = -1;
        double coverTallest = 0;
        for (int counter = 0; counter < sg->ref(xcounter, ycounter).plants.size(); counter++)
        {
            auto& plant = sg->ref(xcounter, ycounter).plants[counter];
            if (plant.calcHeight() > tallest)
            {
                tallestIndex = counter;
                tallest = plant.calcHeight();
            }
            if (plant.vp.isCover)
            {
                if (plant.calcHeight() > coverTallest)
                {
                    coverIndex = counter;
                    coverTallest = plant.calcHeight();
                }
            }
        }
        TCODColor back, fore;
        int icon = 0;

        if (tallestIndex != -1) // -1 : no plants exist
        {
            auto& plant = sg->ref(xcounter, ycounter).plants[tallestIndex];
            icon = plant.vp.icon_mature;
            auto rgb = plant.vp.getColor();
            fore = TCODColor(rgb.r, rgb.g, rgb.b);
        }

        if (coverIndex != -1) // At least 1 plant taller than 0 exists
        {
            auto& plant = sg->ref(xcounter, ycounter).plants[coverIndex];
            auto rgb = plant.vp.getColor();
            back = TCODColor(rgb.r, rgb.g, rgb.b);
            back = TCODColor::lerp(back, TCODColor::black, 0.1f);
        }
        else
        {
            back = ground->getCharBackground(xcounter, ycounter);
        }

        
        if (coverIndex == -1 && tallestIndex == -1) // no plants exist
            back = TCODColor::black;


        plants->putCharEx(xcounter, ycounter, icon, fore, back);
    }

    TCODConsole::blit(ground, 0, 0, 0, 0, console, 0, 0);
    TCODConsole::blit(plants, 0, 0, 0, 0, console, 0, 0);
}

MapScreen::~MapScreen()
{
    if (console)
        delete console;
    if (ground)
        delete ground;
    if (plants)
        delete plants;
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
        auto soilgroup = cell.getTopsoilGroup();
        auto soiltype = cell.getTopsoilType();
        auto soilname = soilDict.getSoilName(soiltype);
        TCODColor fore = soilDict.getBackColor(soiltype);
        messages.push_back(ColoredMessage(soilname, fore));

        for (auto plant : sg->ref(abscoord.first, abscoord.second).plants)
        {
            auto RGB = plant.vp.getColor();
            TCODColor myColor(RGB.r, RGB.g, RGB.b);
            std::string name = plant.vp.name;
            TCODColor back(255, 0, 255);
            if (plant.vp.whiteBackground)
                back = TCODColor::white;
            messages.push_back(ColoredMessage(name, myColor, back));
        }
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