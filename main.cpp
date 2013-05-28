#include "Engine.h"
#include <stdlib.h>

int main(int argc, char **argv)
  {
  srand(time(0));
  DisplayEngine.Init(640,480);
	DisplayEngine.Start();
	DisplayEngine.End();
  return 0;
  }