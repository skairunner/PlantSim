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
ALLEGRO_FONT* font;
SoilGrid* soilGrid;
int mouseX, mouseY;

class SoilColorDict
  {
  public:
    SoilColorDict()  /// MUST BE CREATED __AFTER__ al_primatives IS INSTALLED o_O
      {
      colors.insert(pair<int, ALLEGRO_COLOR>(stCLAY, al_map_rgb(240,190,153)));
      colors.insert(pair<int, ALLEGRO_COLOR>(stSANDYCLAY, al_map_rgb(220, 217, 198)));
      colors.insert(pair<int, ALLEGRO_COLOR>(stSILTYCLAY, al_map_rgb(200, 173, 152)));
      colors.insert(pair<int, ALLEGRO_COLOR>(stCLAYLOAM, al_map_rgb(192, 161, 140)));
      colors.insert(pair<int, ALLEGRO_COLOR>(stSILTYCLAYLOAM, al_map_rgb(187, 174, 168)));
      colors.insert(pair<int, ALLEGRO_COLOR>(stSANDYCLAYLOAM, al_map_rgb(211,193,157)));
      colors.insert(pair<int, ALLEGRO_COLOR>(stLOAM, al_map_rgb(202, 195, 169)));
      colors.insert(pair<int, ALLEGRO_COLOR>(stSILTLOAM, al_map_rgb(227, 226, 214)));
      colors.insert(pair<int, ALLEGRO_COLOR>(stSANDYLOAM, al_map_rgb(209, 204, 162)));
      colors.insert(pair<int, ALLEGRO_COLOR>(stLOAMYSAND, al_map_rgb(194, 187, 159)));
      colors.insert(pair<int, ALLEGRO_COLOR>(stSAND, al_map_rgb(239, 236, 205)));
      colors.insert(pair<int, ALLEGRO_COLOR>(stSILT, al_map_rgb(166, 168, 167)));
      }

   /* ALLEGRO_COLOR editAlCol(const int& r, const int& g, const int& b)
      {
      ALLEGRO_COLOR buffer;
      buffer.r = (double)r / 255;
      buffer.g = (double)g / 255;
      buffer.b = (double)b / 255;
      buffer.a = 1;
      return buffer;
      }*/

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


/*ALLEGRO_COLOR findSoilColor(SoilColorDict& scd, const int soilType)
  {
  return scd.getColor(soilType);
  }*/

bool Engine::EngineInit(map<int,bool> errormap)
  {
  soilGrid = new SoilGrid(64, 64);

  font = al_load_ttf_font("malgun.ttf", 16, 0);
  
  SoilColorDict soilcolorDict;
  bitmap = al_create_bitmap(soilGrid->getWidth(), soilGrid->getHeight());
  al_lock_bitmap(bitmap, al_get_bitmap_format(bitmap), ALLEGRO_LOCK_READWRITE);
  for (int y = 0; y < soilGrid->getHeight(); y++)
    for (int x = 0; x < soilGrid->getWidth(); x++)
      {
      al_put_pixel(x, y, soilcolorDict.getColor(soilGrid->ref(x, y).getTopsoilType()));
      }
    al_unlock_bitmap(bitmap);

  return true;
  }

void Engine::Update()
  {
  }

void Engine::Render(ALLEGRO_DISPLAY *root)
  {

  al_set_target_bitmap(al_get_backbuffer(root));
  al_clear_to_color(al_map_rgb(250, 0,254));
  al_draw_bitmap(bitmap, 0, 0, 0);

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