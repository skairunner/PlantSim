#include "Engine.h"
#include "soil.h"
#include "colors.h"
#include <vector>

using namespace std;
using namespace ALMANAC;

Engine DisplayEngine;
vector<ALLEGRO_BITMAP*> bitmaps;
SoilCell* testCell;
bool nextstep, addmorewater;
int updatecounter;
int watercounter;
ALLEGRO_FONT* font;

ALLEGRO_COLOR findSoilColor(const soiltuple& input)
  {

  }

bool Engine::EngineInit(map<int,bool> errormap)
  {
  testCell = SoilFactory::createTestCell();
  int counter = testCell->inspectWater().size();
  for (;counter > 0; counter--)
    {
    ALLEGRO_BITMAP* temp = al_create_bitmap(100, 20);
    al_set_target_bitmap(temp);
    al_clear_to_color(al_map_rgb(255,0,0));
    bitmaps.push_back(temp);
    }

  testCell->addWater(2,100);
  font = al_load_ttf_font("malgun.ttf", 12, 0);
  nextstep = false;
  addmorewater = false;
  updatecounter = 0;
  watercounter = 0;

  return true;
  }

void Engine::Update()
  {
  if (updatecounter == 10)
    {
    nextstep = false;
    testCell->solveAndPercolate();
    updatecounter = 0;
    }
  else 
    updatecounter++;

  if (watercounter % 80 == 0)
    {
    testCell->addWater(0, 10);
    }
  if (watercounter == 160)
    {
    testCell->addWater(0, 30);
    watercounter = 0;
    }
  

  watercounter++;
  
  }

void Engine::Render(ALLEGRO_DISPLAY *root)
  {
  ALLEGRO_COLOR red = al_map_rgb(200,0,0);
  ALLEGRO_COLOR blue = al_map_rgb(0,0,255);
  ALLEGRO_COLOR temp = al_map_rgb(0,0,0);
  vector<double> waters = testCell->inspectWater();
  for (int counter = 0; counter < bitmaps.size(); counter++)
    {
    al_set_target_bitmap(bitmaps[counter]);
    temp = ColorMath::lerp(red, blue, waters[counter]/150);
    al_clear_to_color(temp);
    al_set_target_bitmap(al_get_backbuffer(root));
    al_draw_bitmap(bitmaps[counter], 0, 20 * counter, 0);
    al_draw_text(font, al_map_rgb(0,0,0), 0, 20 * counter, 0, to_string((long double)waters[counter]).c_str());
    }
  }

void Engine::EngineEnd()
  {
  }

void Engine::KeyUp(const int &key,const int &unicode, const int &mods)
  {
  }

void Engine::KeyDown(const int &key,const int &unicode, const int &mods, const bool &repeated)
  {
  if (key == ALLEGRO_KEY_RIGHT)
    nextstep = true;
  }

void Engine::MouseMoved(const int &iX,const int &iY,const int &iZ, const int &iRelX, const int &iRelY,const int &iRelZ)
  {
  }

void Engine::MouseButtonUp(const int &iButton, const int &iX,const int &iY,const int &iZ, const int &iRelX, const int &iRelY,const int &iRelZ)
  {
  }

void Engine::MouseButtonDown(const int &iButton, const int &iX,const int &iY,const int &iZ, const int &iRelX, const int &iRelY,const int &iRelZ)
  {
  }

void Engine::TimerTick(ALLEGRO_TIMER *timer, int count)
  {
  }