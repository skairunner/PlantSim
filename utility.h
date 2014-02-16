#pragma once
#include <string>

std::string rightAlignNumber(const int& input, const int& size = -1);
std::string rightAlign(const std::string& input, const int& size);
std::string leftAlign(const std::string& input, const int& size);
std::string changeToDecimal(const std::string& input);
int stringToDecimal(const std::string& input);
typedef std::pair<int, int> coord;

//define later
/*extern const int WIDTH;
extern const int HEIGHT;
extern const int screenwidth;
extern const int screenheight;*/