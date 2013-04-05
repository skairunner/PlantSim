#include <iostream>
#include "parser_rain.h"
#include <string>

using namespace std;

int main()
  {
  Parse::Parser_RainyDays filereader(string("Content/unknownRainyDays.rain"));
  bool valid = filereader.load();
  if (!valid)
    cout << "File failed to open.\n";
  else
    {
    filereader.parse();
    vector<int> data = filereader.getResult();
    for (auto it = data.begin(); it < data.end(); it++)
      {
      cout << *it << " ";
      }
    }

  cin.ignore(1);

  return 0;
  }