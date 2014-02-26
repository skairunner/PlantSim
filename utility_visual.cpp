#include "utility_visual.h"

SoilDictionary soilDict;
TCODColor MabinogiBrown = TCODColor(95, 71, 61);
TCODColor DarkMabinogiBrown = TCODColor(71, 53, 46);

ColoredMessage::ColoredMessage()
{

}

ColoredMessage::ColoredMessage(const std::string& content, const TCODColor& forecolor, const TCODColor& backcolor)
{
    text = content;
    fore = forecolor;
    back = backcolor;
}

SoilDictionary::SoilDictionary()
{
    backcolors[stCLAY] = TCODColor(240, 190, 153);
    backcolors[stSANDYCLAY] = TCODColor(220, 217, 198);
    backcolors[stSILTYCLAY] = TCODColor(200, 173, 152);
    backcolors[stCLAYLOAM] = TCODColor(192, 161, 140);
    backcolors[stSILTYCLAYLOAM] = TCODColor(187, 174, 168);
    backcolors[stSANDYCLAYLOAM] = TCODColor(211, 193, 157);
    backcolors[stLOAM] = TCODColor(202, 195, 169);
    backcolors[stSILTLOAM] = TCODColor(227, 226, 214);
    backcolors[stSANDYLOAM] = TCODColor(209, 204, 162);
    backcolors[stLOAMYSAND] = TCODColor(194, 187, 159);
    backcolors[stSAND] = TCODColor(239, 236, 205);
    backcolors[stSILT] = TCODColor(166, 168, 167);

    frontcolors[stgCLAYGROUP] = TCODColor(109, 74, 39);
    frontcolors[stgSILTGROUP] = TCODColor(109, 74, 39);
    frontcolors[stgSANDGROUP] = TCODColor(255, 255, 255);
    frontcolors[stgLOAMGROUP] = TCODColor(109, 74, 39);

    icons[stgCLAYGROUP] = 0;
    icons[stgSILTGROUP] = 176;
    icons[stgSANDGROUP] = 177;
    icons[stgLOAMGROUP] = 176;

    names[stCLAY] = "Clay";
    names[stCLAY] = "Clay";
    names[stSANDYCLAY] = "Sandy clay";
    names[stSILTYCLAY] = "Silty clay";
    names[stCLAYLOAM] = "Clay loam";
    names[stSILTYCLAYLOAM] = "Silty clay loam";
    names[stSANDYCLAYLOAM] = "Sandy clay loam";
    names[stLOAM] = "Loam";
    names[stSILTLOAM] = "Silt loam";
    names[stSANDYLOAM] = "Sandy loam";
    names[stLOAMYSAND] = "Loamy sand";
    names[stSAND] = "Sand";
    names[stSILT] = "Silt";
}

TCODColor SoilDictionary::getBackColor(const int soilType)
{
    return backcolors.find(soilType)->second;
}

TCODColor SoilDictionary::getFrontColor(const int soilGroup)
{
    return frontcolors.find(soilGroup)->second;
}

int SoilDictionary::getIcon(const int soilGroup)
{
    return icons.find(soilGroup)->second;
}

std::string SoilDictionary::getSoilName(int soilType)
{
    return names.find(soilType)->second;
}