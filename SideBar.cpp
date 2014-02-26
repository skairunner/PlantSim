#include "SideBar.h"
#include "utility_visual.h"
#include "TCODTools.h"

using namespace HerbSim;

SideBar::SideBar(int barWidth, int barHeight, TCODColor backgroundColor)
{
    rootX = rootY = 0;
    screenwidth = barWidth;
    screenheight = barHeight;
    console = new TCODConsole(barWidth, barHeight);
    update = true;
    back = backgroundColor;
}

SideBar::~SideBar()
{
    if (console)
        delete console;
}

int SideBar::getUsableWidth()
{
    return screenwidth - 2;
}

void SideBar::Render(TCODConsole *root)
{
    if (update)
        doUpdate();
    TCODConsole::blit(console, 0, 0, screenwidth, screenheight, root, rootX, rootY, 1.0, 1.0);
}

void SideBar::setMessages(const std::vector<ColoredMessage>& messages)
{
    Things = messages;
    update = true;
}

void SideBar::addMessage(const ColoredMessage& message)
{
    Things.push_back(message);
    update = true;
}

void SideBar::doUpdate()
{
    console->clear();
    console->setDefaultForeground(TCODColor::black);
    console->setDefaultBackground(back);
    console->rect(0, 0, screenwidth, screenheight, true, TCOD_BKGND_SET);
    console->printFrame(0, 0, screenwidth, screenheight, false);

    int line = 1;
    for (auto CM : Things)
    {
        if (CM.fore != TCODColor(255, 0, 255))
            console->setDefaultForeground(CM.fore);
        else
            console->setDefaultForeground(TCODColor::white);

        if (CM.back != TCODColor(255, 0, 255))
            console->setDefaultBackground(CM.back);
        else
            console->setDefaultBackground(back);
        console->printRectEx(1, line++, getUsableWidth(), 1, TCOD_BKGND_SET, TCOD_LEFT, CM.text.c_str());
    }

    update = false;
}


