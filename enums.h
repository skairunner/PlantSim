#pragma once

namespace ALMANAC
  {
  enum MONTHS{JANUARY = 1, FEBRUARY, MARCH, APRIL, MAY, JUNE, JULY, AUGUST, SEPTEMBER, OCTOBER, NOVEMBER, DECEMBER};
  enum SOILTYPE {stCLAY = 1, stSANDYCLAY, stSILTYCLAY, stCLAYLOAM, stSILTYCLAYLOAM, stSANDYCLAYLOAM, stLOAM, stSILTLOAM, stSANDYLOAM, stLOAMYSAND, stSAND, stSILT};
  enum SOILTYPEGROUP {stgCLAYGROUP = 1, stgSANDGROUP, stgLOAMGROUP, stgSILTGROUP};
  enum MOORE {NONE = -1, TOP = 0, TOPLEFT, LEFT, BOTTOMLEFT, BOTTOM, BOTTOMRIGHT, RIGHT, TOPRIGHT};
  }