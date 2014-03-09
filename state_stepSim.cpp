#include "state_stepSim.h"
#include "utility_visual.h"
#include "SDL.h"

State_StepSim::State_StepSim(ALMANAC::Weather* weather, ALMANAC::SoilGrid* soilgrid, bool& updateMap, int number)
: multi(false), abort(false), updateMapRef(updateMap)
{
    WeatherModule = weather;
    Grid = soilgrid;
    loading = new TCODConsole(30, 6);
    WeatherModule->step();
    t = new std::thread(&ALMANAC::SoilGrid::stepAll, Grid, WeatherModule->getDataBundle());
    iterations = number;
    if (iterations < 1)
        popMe = true;
    else if (iterations > 1)
    {
        multi = true;
        itleft = iterations;
    }
}

void State_StepSim::End()
{
    if (loading) 
        delete loading;
    if (t)
        delete t;
    updateMapRef = true;
}

void State_StepSim::KeyDown(const int &key, const int &unicode)
{
    if (key == SDLK_ESCAPE)
    {
        abort = true;
    }
}

void State_StepSim::Update()
{
    int progress = Grid->progress;
    int max = Grid->maxprogress;

    loading->clear();
    loading->setDefaultBackground(MabinogiBrown);
    loading->setDefaultForeground(DarkMabinogiBrown);
    loading->printFrame(0, 0, 30, 6, true, TCOD_BKGND_SET);
    loading->setDefaultForeground(TCODColor::white);

    std::string stage;
    std::string number;
    if (progress / (double)max > 0.5)
        stage = "calculating plants";
    else stage = "running soil sim";

    if (multi)
    {
        number = std::to_string(iterations - itleft + 1) + "/" + std::to_string(iterations);
    }

    loading->printEx(1, 1, TCOD_BKGND_DEFAULT, TCOD_LEFT, "%s (%s)", stage.c_str(), number.c_str());
    loading->printEx(21, 2, TCOD_BKGND_DEFAULT, TCOD_LEFT, "%.0f%%", 100 * progress / (double)max);

    int bars = progress / (double)max * 20;
    loading->setDefaultForeground(TCODColor::darkRed);
    for (int counter = 0; counter < 20; counter++)
    {
        loading->putChar(counter + 1, 2, 219);
    }
    for (int counter = 0; counter < bars; counter++)
    {
        loading->setCharForeground(counter + 1, 2, TCODColor::red);
    }

    if (progress == max)
    {
        t->join();
        if (!multi || itleft == 1 || abort)
        {
            popMe = true;
        }
        else
        {
            delete t;
            WeatherModule->step();
            t = new std::thread(&ALMANAC::SoilGrid::stepAll, Grid, WeatherModule->getDataBundle());
            itleft--;
        }
    }
}

void State_StepSim::Render(TCODConsole *root)
{
    int x = root->getWidth() / 2;
    int y = root->getHeight() / 2;
    x -= 15;
    y -= 2;
    TCODConsole::blit(loading, 0, 0, 0, 0, root, x, y);
}