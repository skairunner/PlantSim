//#include "Engine.h"
#include <stdlib.h>
#include "parser_rain.h"
#include <string>


int main(int argc, char **argv)
  {
  Parse::MonolithParse parser(std::string("Content/seosan_skew.monolith"));
  parser.load();
  auto it = parser.parse();

  /*srand(time(0));
  DisplayEngine.Init(1024,512);
	DisplayEngine.Start();
	DisplayEngine.End();*/
  return 0;
  }