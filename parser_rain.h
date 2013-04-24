#pragma once
#include <string>
#include <vector>

// This class parses files with a .rain extension.
// Possible use as other parsers.

namespace Parse
  {
  using namespace std;

  class Parser
    {
    public:
      //Parser();
      Parser(const string& filename);
      bool load();
      vector<vector<double>> parse();
      bool isLoaded();

    private:
      bool loaded;
      virtual vector<double> process(string& input); // Take a string, discard the first cell, then pack all cells except the final cell into a vector of floats.
      //virtual void parseLogic();

      vector<string> rawbuffer;
     // string internalbuffer;
      string fileName;
    };

  class Parser_RainyDays: public Parser
    {
    public:
      Parser_RainyDays(const string& filename);
     // virtual vector<vector<float>> parse();
      vector<int> getResult();

    private:
      virtual vector<double> process(string &input);
      vector<int> result;
    };

  class StatisticsParser
    {
    public:
      void loadData(vector<vector<double>> & input); // Load a vector of all vectors of that month. Consolidates it.
      void loadData(const vector<double>& input); // In case you only have one vector.


      ////
      //// Accessors
      ////
      double getMean();
      double getVariance();
      double getSTDEV();
      double getMedian();
      double getSkew();

    private:
      double mean;
      double median;
      double standardDeviation;
      double variance;
      double skew;

      double findMean();
      double findVariance();
      double findMedian();
      double findSkew();

      vector<double> consolidated;
    };

  class outputColumn
    {
    public:
      void setfilename(const string& filename);     
      void output(vector<double>& outputVector);
      void outputRow(vector<double>& outputVector);
      void clearFile();

    private:
      string file;
    };

  }