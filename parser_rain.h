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
      vector<vector<float>> parse();
      bool isLoaded();

    private:
      bool loaded;
      virtual vector<float> process(string& input); // Take a string, discard the first cell, then pack all cells except the final cell into a vector of floats.
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
      vector<int>& getResult();

    private:
      virtual vector<float> process(string &input);
      vector<int> result;
    };

  class RainStatisticsParser
    {
    public:
      void loadData(vector<vector<float>> & input); // Load a vector of all vectors of that month. Consolidates it.
      void loadData(const vector<float>& input); // In case you only have one vector.


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

      vector<float> consolidated;
    };

  }