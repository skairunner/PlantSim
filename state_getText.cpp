#include "state_getText.h"
#include "utility_visual.h"
#include "SDL.h"
#include "enums.h"


int State_getText::nullstate = 0;
State_getText::State_getText(const int& width, const int& height, const std::string& prompt, std::string& output, int& state)
: w(width), h(height), out(output), stateRef(state)
{
    message = prompt;
    screen = new TCODConsole(w, h);
    redraw();
    redrawNeeded = false;
}

void State_getText::redraw()
{
    screen->setDefaultBackground(MabinogiBrown);
    screen->setDefaultForeground(DarkMabinogiBrown);
    screen->printFrame(0, 0, w, h, true, TCOD_BKGND_SET);
    screen->setDefaultForeground(TCODColor::white);
    screen->printRect(1, 1, w - 2, 1, message.c_str());
    screen->printRect(1, 2, w - 2, h - 3, ("> " + outbuffer).c_str());
}

void State_getText::Render(TCODConsole *root)
{
    TCODConsole::blit(screen, 0, 0, 0, 0, root, 0, 0);
}

void State_getText::Update()
{
    if (redrawNeeded)
    {
        redraw();
        redrawNeeded = false;
    }
     
}

void State_getText::End()
{
    if (screen)
        delete screen;
}

void State_getText::KeyDown(const int &key, const int &unicode)
{
    if (key == SDLK_ESCAPE)
    {
        out = "";
        popMe = true;
        stateRef = promptNONE;
    }
    else if (key == SDLK_RETURN)
    {
        out = outbuffer;
        popMe = true;
        stateRef = promptRECIEVED;
    }
    else if (key == SDLK_BACKSPACE)
    {
        if (outbuffer.size() > 0)
            outbuffer.pop_back();
        redrawNeeded = true;
    }
    else
    {
        outbuffer.push_back(unicode);
        redrawNeeded = true;
    }
}