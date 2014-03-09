#pragma once
#include "GameState.h"

class State_getText : public GameState
{
public:
    static int nullstate;
    State_getText(const int& width, const int& height, const std::string& prompt, std::string& output, int& state = nullstate);
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

protected:
    void redraw();
    std::string& out; // The reference to the output 
    std::string outbuffer; // The internal buffer that is stored, and copied to "out" when the state ends.
    std::string message;
    TCODConsole* screen;
    bool redrawNeeded;
    int w, h;
    int& stateRef;
};