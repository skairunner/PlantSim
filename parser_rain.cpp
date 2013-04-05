#include "parser_rain.h"
#include <fstream>
#include <cmath>
#include <algorithm>

namespace Parse
  {
  /////
  /// Parser (.rain)
  /////

 /* Parser::Parser()
    {}
    */
  Parser::Parser(const string& filename)
    {
    fileName = filename; loaded = false;
    }

  bool Parser::isLoaded()
    {
    return loaded;
    }

  bool Parser::load()
    {
    fstream file;
    string source;
    string buffer;
    file.open(fileName.c_str());
    rawbuffer.clear();

    if(file.is_open())
      {
      bool continuer = true;
      source.clear();
      while (continuer)
        {
        getline(file, buffer, '\n');
        rawbuffer.push_back(buffer);
        /*source.append(buffer);
        source.append("\n");*/

        if (file.eof())
          continuer = false;
        }

      file.close();
      //internalbuffer = source;
      loaded = true;
      return true;
      }
    else 
      {
      file.close();
      loaded = false;
      return false;
      }
    }

  vector<float> Parser::process(string& input)
    {
    string buffer;
    vector<float> output;
    for (auto it = input.begin(); it < input.end(); it++)
      {
      if (*it != ' ' && *it != 9) // If not a delimiter.
        {
        buffer.push_back(*it);
        }
      else
        {
        if (buffer.size() != 0) // If buffer isn't empty...
          output.push_back(atof(buffer.c_str()));
        else
          output.push_back(0);
        buffer.clear(); // Then empty it.
        }
      }

    // First, discard the last.
    if (output.size() != 0)
      output.pop_back();

    // Next, discard the first.
    if (output.size() != 0)
      output.erase(output.begin());

    return output;
    }

  vector<vector<float>> Parser::parse()
    {
    vector<vector<float>> output;

    for (auto it = rawbuffer.begin()+1; it < rawbuffer.end(); it++) // Skip the first Korean line.
      output.push_back(process(*it));

    return output;
    }

  /////
  /// Wet days / month parser
  /////

  Parser_RainyDays::Parser_RainyDays(const string& filename)
    :Parser(filename)
    {

    }

  /**
  *
  * Expected format: The first line is empty. The second line has one zero followed by twelve integers 
  * corresponding to the number of rainy days in that month. The final integer must be followed by a delimiter.
  *
  */

  vector<float> Parser_RainyDays::process(string &input)
    {
    string buffer;
    vector<float> output;

    for (auto it = input.begin(); it < input.end(); it++)
      {
      if (*it != ' ' && *it != 9) // If not a delimiter.
        {
        buffer.push_back(*it);
        }
      else
        {
        if (buffer.size() != 0) // If buffer isn't empty...
          {
          output.push_back(atof(buffer.c_str()));
          result.push_back(atoi(buffer.c_str()));
          }
        else
          output.push_back(0);
        buffer.clear(); // Then empty it.
        }
      }

    return output;
    }

  vector<int>& Parser_RainyDays::getResult()
    {
    return result;
    }

  /////
  /// Rain stats parser
  /////

  void RainStatisticsParser::loadData(const vector<float> & input)
    {
    consolidated = input;
    mean = findMean();
    variance = findVariance();
    skew = findSkew();
    median = findMedian();
    standardDeviation = sqrt(variance);
    }

  double RainStatisticsParser::findMean()
    {
    double sum = 0;
    for (auto it = consolidated.begin(); it < consolidated.end(); it++)
      {
      sum += *it;
      }

    if (consolidated.size() == 0)
      return 0;
    else
      return sum / consolidated.size();
    }

  double RainStatisticsParser::findVariance()
    {
    double sum = 0;
    for (auto it = consolidated.begin(); it < consolidated.end(); it++)
      {
      sum += pow(*it - mean, 2);
      }
    sum /= consolidated.size();
    return sum;
    }

  double RainStatisticsParser::findMedian()
    {
    std::sort(consolidated.begin(), consolidated.end());
    int position;
    if (consolidated.size() % 2 == 0) // if even
      {
      position = consolidated.size() / 2 - 1;
      double result = consolidated[position-1] + consolidated[position];
      return result/2;
      }
    else // if odd
      {
      position = consolidated.size() / 2; // eg, if 31 numbers, returns [7].
      return consolidated[position];
      }

    }

  double RainStatisticsParser::findSkew()
    {
    return 3.0f * (mean - median) / getSTDEV();
    }

  double RainStatisticsParser::getMean()
    {
    return mean;
    }

  double RainStatisticsParser::getVariance()
    {
    return variance;
    }

  double RainStatisticsParser::getSkew()
    {
    return skew;
    }

  double RainStatisticsParser::getSTDEV()
    {
    return sqrt(variance);
    }

  double RainStatisticsParser::getMedian()
    {
    return median;
    }
  }