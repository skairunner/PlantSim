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

  vector<double> Parser::process(string& input)
    {
    string buffer;
    vector<double> output;
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

    
    /*// Next, discard the first.
    if (output.size() != 0)
      output.erase(output.begin());*/
    ///// Edited to not discard the first, as to provide padding.

    return output;
    }

  vector<vector<double>> Parser::parse()
    {
    vector<vector<double>> output;

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

  vector<double> Parser_RainyDays::process(string &input)
    {
    string buffer;
    vector<double> output;

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

  vector<int> Parser_RainyDays::getResult()
    {
    return result;
    }

  /////
  /// Rain stats parser
  /////

  void StatisticsParser::loadData(const vector<double> & input)
    {
    consolidated = input;
    mean = findMean();
    variance = findVariance();
    standardDeviation = sqrt(variance);
    skew = findSkew();
    median = findMedian();
    }

  double StatisticsParser::findMean()
    {
    double sum = 0;
    for (auto it = consolidated.begin() + 1; it < consolidated.end(); it++)
      {
      sum += *it;
      }

    if (consolidated.size() == 0)
      return 0;
    else
      return sum / ((consolidated.size() - 1)==1?1:(consolidated.size()-1));
    }

  double StatisticsParser::findVariance()
    {
    double sum = 0;
    int counter = 0;
    for (auto it = consolidated.begin() + 1; it < consolidated.end(); it++)
      {
      sum += pow(*it - mean, 2);
      counter++;
      }
    sum = sum / (counter - 1);
    return sum;
    }

  double StatisticsParser::findMedian()
    {
    vector<double> temp = consolidated;
    std::sort(temp.begin(), temp.end());
    int position;
    if (temp.size() % 2 == 0) // if even
      {
      position = temp.size() / 2 - 1;
      double result = temp[position-1] + temp[position];
      return result/2;
      }
    else // if odd
      {
      position = temp.size() / 2; // eg, if 31 numbers, returns [7].
      return temp[position];
      }

    }

  double StatisticsParser::findSkew()
    {
    vector<long double> devFromMean;
    vector<double> copyOfConsol;
    for (auto it = consolidated.begin()+1; it < consolidated.end(); it++) // Skip the first.
      copyOfConsol.push_back(*it);
    std::sort(copyOfConsol.begin(), copyOfConsol.end());

    for (auto it = copyOfConsol.begin(); it < copyOfConsol.end(); it++)
      devFromMean.push_back(*it - mean);
    for (auto it = devFromMean.begin(); it < devFromMean.end(); it++)
      {
      *it /= standardDeviation;
      *it = pow(*it, 3); // it ^3
      }
    long double output = 0;
    std::sort(devFromMean.begin(), devFromMean.end());
    for (auto it = devFromMean.begin(); it < devFromMean.end(); it++)
      output += *it;

//    output /= pow(devFromMean.size() - 1, 3.0f);
    long double coef = devFromMean.size();
    coef = coef / (coef - 1) / (coef - 2);
    output *= coef;
    return output;
    }

  double StatisticsParser::getMean()
    {
    return mean;
    }

  double StatisticsParser::getVariance()
    {
    return variance;
    }

  double StatisticsParser::getSkew()
    {
    return skew;
    }

  double StatisticsParser::getSTDEV()
    {
    return sqrt(variance);
    }

  double StatisticsParser::getMedian()
    {
    return median;
    }

  /////
  /// Output a column
  /////

  void outputColumn::setfilename(const string& filename)
    {
    file = filename;
    }    

  void outputColumn::output(vector<double>& outputVector)
          {
        fstream out;
        out.open(file.c_str(), ios::out | ios::trunc);
        for (auto it = outputVector.begin(); it < outputVector.end(); it++)
          {
          out << *it << "\n";
          }
        out.close();
        }

  void outputColumn::outputRow(vector<double>& outputVector)
    {
    fstream out;
        out.open(file.c_str(), ios::out | ios::app);
        for (auto it = outputVector.begin(); it < outputVector.end(); it++)
          {
          out << *it << '\t'; // horizontal tab
          }
        out << "\n";
        out.close();
    }

  void outputColumn::clearFile()
    {
    fstream out;
    out.open(file.c_str(), ios::out | ios::trunc);
    out.close();
    }


  /////
  /// Parse monolith file.
  /////
  MonolithParse::MonolithParse(const string& input)
    : Parser(input)
    {}

  vector<double> MonolithParse::process(string& input)
    {
    string buffer;
    vector<double> output;
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

    return output;
    }

  vector<vector<double>> MonolithParse::parse()
    {
    vector<vector<double>> output;
    output.push_back(vector<double>());

    for (auto it = rawbuffer.begin()+1; it < rawbuffer.end(); it++) // Skip the first Korean line.
      output.push_back(process(*it));

    return output;
    }
  }
