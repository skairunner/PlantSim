#pragma once
#include "libtcod.hpp"
#include <vector>
#include <string>
#include "utility.h"
#include "VisualProperties.h"

namespace HerbSim
{
    class SideBar
    {
    public:
        SideBar(int barWidth, int barHeight, TCODColor backgroundColor);
        ~SideBar();
        TCODColor back;

        void setMessages(const std::vector<ColoredMessage>& messages);
        void addMessage(const ColoredMessage& message);

        int screenwidth;
        int screenheight;
        int getUsableWidth();
        int rootX, rootY;
        TCODConsole* console;
        bool update;

        void doUpdate();
        void Render(TCODConsole *root);

    protected:
        std::vector<ColoredMessage> Things; // The things on the selected tile, or perhaps menu options.
    };
}