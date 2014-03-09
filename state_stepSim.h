#pragma once
#include "GameState.h"
#include "soilGrid.h"
#include "Weather.h"
#include <thread>

class State_StepSim : public GameState
{
public:
    State_StepSim(ALMANAC::Weather* weather, ALMANAC::SoilGrid* soilgrid, bool& updateMap, int number=1);
    virtual bool Init(){ return true; }

    virtual void Update();
    virtual void Render(TCODConsole *root);
    virtual void End();

    //
    virtual void Resize(int new_w, int new_h){}
    virtual void WindowActive(){}
    virtual void WindowInactive(){}
    virtual void KeyUp(const int &key, const int &unicode){}
    virtual void KeyDown(const int &key, const int &unicode);
    virtual void MouseMoved(const int &iButton, const int &iX, const int &iY, const int &iRelX, const int &iRelY){}
    virtual void MouseButtonUp(const int &iButton, const int &iX, const int &iY, const int &iRelX, const int &iRelY){}
    virtual void MouseButtonDown(const int &iButton, const int &iX, const int &iY, const int &iRelX, const int &iRelY){}

    TCODConsole* loading;
protected:
    int iterations;
    int itleft;
    bool& updateMapRef;
    bool multi;
    bool abort;
    ALMANAC::Weather* WeatherModule;
    ALMANAC::SoilGrid* Grid;
    std::thread* t;
};