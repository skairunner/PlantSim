#include "Engine.h"
#include "soil.h"
#include "colors.h"
#include <vector>
#include <map>
#include "soilGrid.h"
#include "enums.h"
#include <string>

using namespace std;
using namespace ALMANAC;

Engine DisplayEngine;
ALLEGRO_BITMAP* bitmap;
ALLEGRO_BITMAP* watermap;
ALLEGRO_FONT* font;
SoilGrid* soilGrid;
int mouseX, mouseY;

class SoilColorDict
  {
  public:
    SoilColorDict()  /// MUST BE CREATED __AFTER__ al_primatives IS INSTALLED o_O
      {
      colors[stCLAY] = al_map_rgb(240,190,153);
      colors[stSANDYCLAY] = al_map_rgb(220, 217, 198);
      colors[stSILTYCLAY] = al_map_rgb(200, 173, 152);
      colors[stCLAYLOAM] = al_map_rgb(192, 161, 140);
      colors[stSILTYCLAYLOAM] = al_map_rgb(187, 174, 168);
      colors[stSANDYCLAYLOAM] = al_map_rgb(211,193,157);
      colors[stLOAM] = al_map_rgb(202, 195, 169);
      colors[stSILTLOAM] = al_map_rgb(227, 226, 214);
      colors[stSANDYLOAM] = al_map_rgb(209, 204, 162);
      colors[stLOAMYSAND] = al_map_rgb(194, 187, 159);
      colors[stSAND] = al_map_rgb(239, 236, 205);
      colors[stSILT] = al_map_rgb(166, 168, 167);
      }

    ALLEGRO_COLOR getColor(const int soilType)
      {
      return colors.find(soilType)->second;
      }
  private:
    map<int, ALLEGRO_COLOR> colors;
  };
class SoilNameDict
  {
  public:
    SoilNameDict()
      {
      std::string buffer;
      names.insert(pair<int,std::string>(stCLAY, std::string("Clay")));
      names.insert(pair<int,std::string>(stSANDYCLAY, std::string("Sandy clay")));
      names.insert(pair<int,std::string>(stSILTYCLAY, std::string("Silty clay")));
      names.insert(pair<int,std::string>(stCLAYLOAM, std::string("Clay loam")));
      names.insert(pair<int,std::string>(stSILTYCLAYLOAM, std::string("Silty clay loam")));
      names.insert(pair<int,std::string>(stSANDYCLAYLOAM, std::string("Sandy clay loam")));
      names.insert(pair<int,std::string>(stLOAM, std::string("Loam")));
      names.insert(pair<int,std::string>(stSILTLOAM, std::string("Silt loam")));
      names.insert(pair<int,std::string>(stSANDYLOAM, std::string("Sandy loam")));
      names.insert(pair<int,std::string>(stLOAMYSAND, std::string("Loamy sand")));
      names.insert(pair<int,std::string>(stSAND, std::string("Sand")));
      names.insert(pair<int,std::string>(stSILT, std::string("Silt")));
      }

    std::string getSoilName(int soilType)
      {
      return names.find(soilType)->second;
      }

  private:
    map<int, std::string> names;

  };

SoilNameDict soilnameDict;

bool Engine::EngineInit(map<int,bool> errormap)
  {
  soilGrid = new SoilGrid(128, 128, 0);

  font = al_load_ttf_font("malgun.ttf", 16, 0);
  
  ALLEGRO_COLOR buffer;
  ALLEGRO_COLOR black = al_map_rgb(50,50,50);
  SoilColorDict soilcolorDict;
  bitmap = al_create_bitmap(soilGrid->getWidth(), soilGrid->getHeight());
  al_set_target_bitmap(bitmap);
  al_lock_bitmap(bitmap, al_get_bitmap_format(bitmap), ALLEGRO_LOCK_READWRITE);
  for (int y = 0; y < soilGrid->getHeight(); y++)
    for (int x = 0; x < soilGrid->getWidth(); x++)
      {
      buffer = soilcolorDict.getColor(soilGrid->ref(x, y).getTopsoilType());
      buffer = ColorMath::lerp(black, buffer, soilGrid->ref(x, y).getTotalHeight()/12000);
      al_put_pixel(x, y, buffer);
      }
    al_unlock_bitmap(bitmap);

  watermap = al_create_bitmap(soilGrid->getWidth(), soilGrid->getHeight());
  al_set_target_bitmap(watermap);
  al_lock_bitmap(watermap, al_get_bitmap_format(watermap), ALLEGRO_LOCK_READWRITE);
  double waterlevel = 0;
  ALLEGRO_COLOR blue = al_map_rgb(0,255,0);
   for (int y = 0; y < soilGrid->getHeight(); y++)
    for (int x = 0; x < soilGrid->getWidth(); x++)
      {
      waterlevel = soilGrid->ref(x, y).inspectWater().front();
      buffer = ColorMath::lerp(black, blue, waterlevel/100.0f);
      al_put_pixel(x, y, buffer);
      }
    al_unlock_bitmap(bitmap);

  return true;
  }

void Engine::Update()
  {
  soilGrid->step();
  }

void Engine::Render(ALLEGRO_DISPLAY *root)
  {
  al_set_target_bitmap(watermap);
  al_lock_bitmap(watermap, al_get_bitmap_format(watermap), ALLEGRO_LOCK_READWRITE);
  double waterlevel = 0;
  ALLEGRO_COLOR blue = al_map_rgb(0,0,255);
  ALLEGRO_COLOR black = al_map_rgb(0,0,0);
  ALLEGRO_COLOR buffer;
   for (int y = 0; y < soilGrid->getHeight(); y++)
    for (int x = 0; x < soilGrid->getWidth(); x++)
      {
      waterlevel = soilGrid->ref(x, y).inspectWater().front();
      buffer = ColorMath::lerp(black, blue, waterlevel/100.0f);
      al_put_pixel(x, y, buffer);
      }
    al_unlock_bitmap(watermap);

    al_set_target_bitmap(al_get_backbuffer(root));
    al_clear_to_color(al_map_rgb(250, 0,254));
    al_draw_bitmap(bitmap, 0, 0, 0);
    al_draw_bitmap(watermap, soilGrid->getWidth(), 0, 0);

  if (mouseX > soilGrid->getWidth() -1 || mouseY > soilGrid->getHeight()-1 || mouseX < 0 || mouseY < 0)
    al_draw_text(font, al_map_rgb(255,254,253), 0, soilGrid->getHeight() + font->height, 0, "Nothing");
  else
    {
    al_draw_text(font, al_map_rgb(0,0,0), 0, soilGrid->getHeight() + font->height, 0, soilnameDict.getSoilName(soilGrid->get(mouseX, mouseY).getTopsoilType()).c_str());
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

  }

void Engine::MouseMoved(const int &iX,const int &iY,const int &iZ, const int &iRelX, const int &iRelY,const int &iRelZ)
  {
  mouseX = iX;
  mouseY = iY;
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