#include <stdlib.h>
#include <iostream>
#include "Engine_tcod.h"


using namespace std;

#undef main

int main(int argc, char **argv)
{
    // vector<string> list = { "fescue grass", "fescue grass", "fescue grass", "fescue grass", "oak" };
    /*vector<string> list = { "oak", "oak" };
    Tests::multiplePlants(360 * 50, list);*/
    
    CursesEngine.Init("Prealpha version 0.0.1", "Content/asciisquare.bmp", 100, 48);
    CursesEngine.Start();
    CursesEngine.End();

    cin.ignore(0);
    return 0;
}