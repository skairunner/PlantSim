#pragma once
#include <vector>

namespace ALMANAC
{
    class StatsHolder // Stores the means to be retrieved per day, the stdev/total mean/median/etc etc. for one month.
    {
    public:
        StatsHolder(const int& TheMonth, const double& Mean, const double& standardDev, const double& Skew, const std::vector<double>& MeanVector)
            :thisMonth(TheMonth), mean(Mean), standardDeviation(standardDev), skew(Skew), means(MeanVector)
        {
        }

        StatsHolder() :mean(0), thisMonth(0), standardDeviation(0), skew(0){}

        int thisMonth;
        double mean, standardDeviation, skew;
        std::vector<double> means; // the day referenced when doing rain calcs
    };


}