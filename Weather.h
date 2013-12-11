#pragma once
#include <noise.h>
#include "boost/random/mersenne_twister.hpp"
#include "boost/random/normal_distribution.hpp"
#include <vector>
#include "Months.h"
#include "rain_stats_holder.h"

namespace ALMANAC
{
    using namespace std;

    struct WeatherData
    {
        WeatherData();
        WeatherData(const double& MaxTemp, const double& MinTemp, const double& Radiation, const double& CO2ppm, const double& Humidity, const double& MeanWindSpeed);
        double maxTemp, minTemp, radiation, CO2, humidity, meanWindSpeed;
        double nightLength;

        Month date;
    };

    ///
    ///
    /// Other things to do include: wind speed functions and .
    ///
    ///

    class Weather
    {
    public:
        Weather(const vector<int>& rainyDaysPerMonth, const bool random = false);
        Weather(bool def);
        bool loadRain(const vector<vector<double>>& rainMeans, const vector<vector<double>>& stdevs, const vector<vector<double>>& skews); /// stdevs is from the monolith file with rain as the fifth column.
        bool loadTemps(const vector<vector<double>>& tempHiMeans, const vector<vector<double>>& tempLowMeans, const vector<vector<double>>& STDEVs);
        bool loadSun(const vector<vector<double>>& sunlightMean);
        bool loadSTDEVs(const vector<vector<double>>& STDEVs);
        bool loadHumidity(const vector<vector<double>>& Humidness);
        bool loadSkew(const vector<vector<double>>& skewVec);

        void step(const float& timestep = 1);
        bool rain();

        double getRainAmount();
        double getMaxTemp();
        double getMinTemp();
        double getDayRadiation();
        double getAverageHumidity();
        double getNightLength();
        WeatherData getDataBundle();

        Month getMonth();
        void changeDate(const int month, const int date);
        void changeDate(Month& Date);


        double latitude; // radians
        const static double earthAxis; // radians

        //////////////////////////
    private:
        boost::random::mt19937 gen;
        vector<int> wetDaysPerMonth; // <month, number of wet days>

        bool loadedRain, loadedTemp, loadedSun;

        Month currentMonth;
        vector<StatsHolder> RainHolder;
        vector<StatsHolder> MaxTemp;
        vector<StatsHolder> MinTemp;
        vector<StatsHolder> sunlight;
        vector<StatsHolder> MeanHumidity;

        ///
        //// Rain
        ///
        void checkForRain();
        void setRainAmount();
        bool rainedYesterday, rainedToday;
        double rainedHowMuch;
        double rainCoef;
        ///
        //// Sun
        ///

        void findTemp();
        void findRadiation();
        double minTemp, maxTemp, dayRadiation;
        double directSun;
        double diffuseSun;
        double omegaT, // How much wet days affect temp. 0.5
            omegaR, // 0.5
            defaultRadiation; // W / m^2, currently 445

        ///
        //// Humidity
        ///
        void findHumidity();
        double omegaH; // how much wet days influence humidity. Usually 0.9
        double humidity;
        ///
        //// RNGs
        ///
        int random(const int& min, const int& max);
        double random(const double& min, const double& max);
        double normalRandom(); // Returns a normally distributed random number. mean 0 std 1
        double normalRandom(const double& mean, const double& stdev); // Returns a normally distributed random number with specified mean & stdev
    };
}