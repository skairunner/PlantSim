#include "soilGrid.h"
#include <stdlib.h>
#include <vector>
#include "enums.h"
#include <iostream>
#include <cstdlib>
#include "Weather.h"
#include "plantDictionary.h"
#include <iostream>

using namespace ALMANAC;

void transferWater::transfer(SoilCell& waterOut, SoilCell& destination)
{
    if (waterOut.Layers.size() != destination.Layers.size())
    {
        std::cerr << "Layer sizes do not match.";
        abort();
    }
    else
    {
        auto destit = destination.Layers.begin();
        for (auto it = waterOut.Layers.begin(); it < waterOut.Layers.end(); it++)
        {
            destit->addWater(it->lateral);
            destit++;
        }
    }
}

void SoilGrid::setPerlinProperties(noise::module::Perlin& gen, const double& freq, const double& lacturnity, const int& octave, const double&persist)
{
    if (freq != 0)
        gen.SetFrequency(freq);
    if (lacturnity != 0)
        gen.SetLacunarity(lacturnity);
    if (octave != 0)
        gen.SetOctaveCount(octave);
    if (persist != 0)
        gen.SetPersistence(persist);
}

double max(const double& left, const double& right)
{
    return left > right ? left : right;
}

SoilGrid::SoilGrid(const int& w, const int& h, unsigned int seed)
:width(w), height(h)
{
    test_totalrad = 0;
    if (!seed)
    {
        rand(); rand(); rand(); rand(); rand(); rand();
        seed = rand();
    }
    perlin.SetSeed(seed);
    sand.SetSeed(seed + 5); // Just random differences.
    clay.SetSeed(seed + 32);
    silt.SetSeed(seed + 129);
    aquifer.SetSeed(seed + 359);

    setPerlinProperties(perlin, 0, 0, 0, 0);
    setPerlinProperties(sand, 0, 0, 0, 0);
    setPerlinProperties(clay, 0, 0, 0, 0);
    setPerlinProperties(silt, 0, 0, 0, 0);
    setPerlinProperties(aquifer, 0, 0, 0, 0);

    double zoom = 0.009;
    double soilZoom = 0.001;
    double startingZ = 0.99;
    SoilCell buffer;
    double baseheight, total;

    const int layers = 10;


    std::vector<soiltuple> soils(layers, soiltuple());
    grid = std::vector<SoilCell>(width * height, SoilCell());
    for (int y = 0; y < height; y++)
    for (int x = 0; x < width; x++)
    {
        soiltuple stBuffer;

        baseheight = (perlin.GetValue(x * zoom, y * zoom, 0.5) + 1) * 5000; // *5000 to get a range of roughly ten meters.
        double aquiferNumber = (perlin.GetValue(x * zoom, y * zoom, 0.5) + 1) * 1000 + 2500; // 2~4 m
        for (int counter = layers - 1; counter >= 0; counter--)
        {
            stBuffer.sand = (sand.GetValue(x * zoom, y * zoom, startingZ - counter * soilZoom) + 1) / 2.5f;
            stBuffer.clay = (clay.GetValue(x * zoom, y * zoom, startingZ - counter * soilZoom) + 1) / 3;
            stBuffer.silt = (silt.GetValue(x * zoom, y * zoom, startingZ - counter * soilZoom) + 1) / 3;
            

            stBuffer.sand = max(stBuffer.sand, 0);
            stBuffer.clay = max(stBuffer.clay, 0);
            stBuffer.silt = max(stBuffer.silt, 0);

            total = stBuffer.sand + stBuffer.clay + stBuffer.silt; // Divide to get the percentage ratios.
            stBuffer.sand /= total;
            stBuffer.silt /= total;
            stBuffer.clay /= total;

            soils[counter] = stBuffer;
        }

        grid[x + y * width] = SoilFactory::createCell(baseheight, 200, soils);
        grid[x + y * width].surfaceWater = 0;

        if (baseheight < aquiferNumber)
            grid[x + y * width].test_isUnderWater = true;
        else
            grid[x + y * width].test_isUnderWater = false;
    }

    for (int y = 0; y < height; y++)
    for (int x = 0; x < width; x++)
    {
        vector3 vecbuffer = findGradientVector(x, y);
        grid[x + y * width].setMooreDirection(findMooreDirection(vecbuffer));
        grid[x + y * width].slope = vecbuffer.length;
        if (vecbuffer.length != vecbuffer.length)
            grid[x + y * width].slope = 0.0001f;
    }
}

void SoilGrid::initGridWithPlant(std::string plantID)
{
    ALMANAC::PlantProperties props = PD.getPlant(plantID);

    for (int xcounter = 0; xcounter < getWidth(); xcounter++)
    for (int ycounter = 0; ycounter < getHeight(); ycounter++)
    {
        BasePlant BP(props, &ref(xcounter, ycounter));
        ref(xcounter, ycounter).plants.push_back(BP);
    }
}

SoilCell SoilGrid::get(const int& x, const int& y)
{
    if (x >= width || y >= height || x < 0 || y < 0)
        return null;
    else
        return grid[x + y * width];
}

SoilCell& SoilGrid::ref(const int& x, const int& y)
{
    if (x >= width || y >= height || x < 0 || y < 0)
        return null;
    else
        return grid[x + y * width];
}

void SoilGrid::set(const int& x, const int& y, const SoilCell& in)
{
    if (x >= width || y >= height || x < 0 || y < 0)
        ;
    else
        grid[x + y * width] = in;
}

int SoilGrid::getWidth()
{
    return width;
}

int SoilGrid::getHeight()
{
    return height;
}

vector3 SoilGrid::findMooreNeighborVector(const int& x, const int& y, const int& neighbor)
{
    SoilCell* copy;
    SoilCell* neighborCell;
    copy = &ref(x, y);
    int dX = 0, dY = 0; // deltaX, deltaY
    if (neighbor == TOPLEFT || neighbor == LEFT || neighbor == BOTTOMLEFT)
        dX -= 1;
    else if (neighbor == BOTTOMRIGHT || neighbor == RIGHT || neighbor == TOPRIGHT)
        dX += 1;
    if (neighbor == TOP || neighbor == TOPLEFT || neighbor == TOPRIGHT)
        dY -= 1;
    else if (neighbor == BOTTOMLEFT || neighbor == BOTTOM || neighbor == BOTTOMRIGHT)
        dY += 1;
    neighborCell = &ref(x + dX, y + dY);
    if (neighborCell == &null) // if out of bounds
    {
        return vector3(0, 0, 0);
    }
    //since it's not o.o.b, check if the other cell is taller than me.
    if (neighborCell->getTotalHeight() > copy->getTotalHeight())
        return vector3(0, 0, 0);

    //since it's not taller, finally return the gradient.
    vector3 out(dX, dY, 0);
    out *= copy->getTotalHeight() - neighborCell->getTotalHeight();
    return out;
}

vector3 SoilGrid::findGradientVector(const int& x, const int& y)
{
    std::vector<vector3> vectors(8, vector3());
    std::vector<vector3> gradients;
    for (int counter = 0; counter < 8; counter++)
        vectors[counter] = findMooreNeighborVector(x, y, counter);
    vector3 total; int numberOfGradients = 0;
    for (auto it = vectors.begin(); it < vectors.end(); it++)
    {
        total += *it;
        if (it->length != 0)
            numberOfGradients++;
    }
    total /= (double)numberOfGradients;
    ref(x, y).gradientVector = total;

    return total;
}

int SoilGrid::findMooreDirection(vector3 input)
{
    vector3 north(0, -1, 0);
    double angle = acos(vector3::dot(north, input) / (north.length * input.length));
    if (angle != angle) // if NaN
        return -1;
    int neighborNumber = 0;
    while (angle > 3.1415f / 4 * neighborNumber + 3.1415f / 8)
    {
        neighborNumber++;
    }
    return neighborNumber % 8;
}

SoilCell* SoilGrid::findMooreNeighbor(const int& x, const int& y, const int& neighbor)
{
    if (neighbor >= 0)
    {
        int dX = 0, dY = 0; // deltaX, deltaY
        if (neighbor == TOPLEFT || neighbor == LEFT || neighbor == BOTTOMLEFT)
            dX -= 1;
        else if (neighbor == BOTTOMRIGHT || neighbor == RIGHT || neighbor == TOPRIGHT)
            dX += 1;
        if (neighbor == TOP || neighbor == TOPLEFT || neighbor == TOPRIGHT)
            dY -= 1;
        else if (neighbor == BOTTOMLEFT || neighbor == BOTTOM || neighbor == BOTTOMRIGHT)
            dY += 1;

        return &ref(x + dX, y + dY);
    }
    return &null;
}

void SoilGrid::doLateralForEachCell()
{
    for (int y = 0; y < height; y++)
    for (int x = 0; x < width; x++)
    {
        SoilCell* neighbor = findMooreNeighbor(x, y, grid[x + y * width].getMooreDirection());
        if (neighbor == &null)
            continue; // if equals null, skip.
        else
            transferWater::transfer(*neighbor, grid[x + y * width]);
    }
}

void SoilGrid::step(const WeatherData& wd)
{
    double rainfall = wd.precipitation;
    double temp = (wd.maxTemp + wd.minTemp) / 2.0;
    
    for (auto it = grid.begin(); it < grid.end(); it++)
    {
        if (rainfall > 0)
            it->addNitrogenToTop(0.0219 * rainfall);
        if (temp < 0)
            it->snow += rainfall;
        else        
            it->surfaceWater += rainfall;

        it->doSnowmelt(temp);
        it->solveAndPercolate();
        it->calculateNitrogen(temp);

        it->surfaceWater -= 3.675247456; // Using the max potential soil evaporation constant for now.
        if (it->surfaceWater < 0)
            it->surfaceWater = 0;
    }
    doLateralForEachCell();
}

void SoilGrid::stepSurfaceFlow(const WeatherData& wd, double timestep)
{
    // First set the flow amounts accordingly.
    for (int x = 0; x < width; x++)
    for (int y = 0; y < height; y++)
    {
        SoilCell* neighbor = findMooreNeighbor(x, y, grid[x + y * width].getMooreDirection());
        SoilCell& current = ref(x, y);
        if (neighbor == &null)
            continue; // if equals null (ie, does not exist), skip.
        double myTotalHeight = current.getTotalHeight() + current.surfaceWater;
        double neighborTotalHeight = neighbor->getTotalHeight() + neighbor->surfaceWater;
        double diff = myTotalHeight - neighborTotalHeight;
        if (diff < 0)
            continue; // if my neighbor is taller than me, skip.
        if (diff > current.surfaceWater)
            diff = current.surfaceWater;
        current.flowAmount = diff * timestep;
        current.surfaceWater -= current.flowAmount;
        // Now, set the <Moore Direction>th element in the neighbor's flowInputs[] array to the diff. 
        // This should prevent collisions, since there is only one cell that has each Moore Direction relative to that cell:
        /*
        eg
        Cell 1 MD<7>   Cell 2 MD<6>   Cell 3 MD<5>

        Cell 4 MD<4>   Neighbor       Cell 5 MD<3>

        Cell 6 MD<2>   Cell 7 MD<1>   Cell 8 MD<0>   
        */

        neighbor->flowInputs[current.getMooreDirection()] = diff * timestep;
    }

    // For each cell, sum the inputs and add them to current surface water.
    for (int x = 0; x < width; x++)
    for (int y = 0; y < height; y++)
    {
        SoilCell& current = ref(x, y);
        double deltaW = current.flowInputs[0];
        current.flowInputs[0] = 0;
        for (int counter = 1; counter < 8; counter++)
        {
            deltaW += current.flowInputs[counter];
            current.flowInputs[counter] = 0;
        }
        current.surfaceWater += deltaW;
    }
}

void SoilGrid::stepPlants(const WeatherData& wd)
{
    test_totalrad = 0;
    radPerPlant.clear();
    for (auto it = grid.begin(); it < grid.end(); it++)
    {
        double totalRad = wd.radiation; 
        const double groundFraction = 1.0;
        vector<double> intervals = { 0.0, 300, 600, 1000, 2000, 3000, 5000, 2000000 }; // does not simulate heights larger than 2 km
        vector<double> rad(it->plants.size(), 0);

        for (int counter = intervals.size() - 1; counter >= 1; counter--)
        {
            double consumedRad = 0;
            double upperLimit = intervals[counter]; // Starts from the last element in intervals
            double lowerLimit = intervals[counter - 1]; // Ends at 0.
            double totalLAI = groundFraction;
            if (counter == 1)
                totalLAI = 0.01;
            for (auto plant = it->plants.begin(); plant < it->plants.end(); plant++)
                totalLAI += plant->getLAI() * plant->prop.LAIGraph.getPositiveArea(lowerLimit, upperLimit, plant->calcHeight()); // Add up the total LAI in the given interval 
            for (int counter = 0; counter < it->plants.size(); counter++)
            {
                auto& plant = it->plants[counter];
                // Give each plant its fraction of the radiation.
                double deltaRad = totalRad * plant.getLAI() * plant.prop.LAIGraph.getPositiveArea(lowerLimit, upperLimit, plant.calcHeight()) / totalLAI;
                rad[counter] += deltaRad;
                consumedRad += deltaRad;
            }
            totalRad -= consumedRad; // Subtract the total taken rad and repeat.
        }

        for (double d : rad)
        {
            test_totalrad += d;
        }

        int plantCounter = 0;
        radPerPlant = rad;
        for (auto plant = it->plants.begin(); plant < it->plants.end(); plant++)
        {
            double radPortion = rad[plantCounter];
            plant->calculate(wd, 0.25, radPortion);


            // Collect seeds
            if (plant->seedlist.size() > 0)
            {
                
                /*for (Seed s : plant->seedlist)
                {
                    it->seeds.push_back(s);
                }*/
                // For testing purposes, only push back one seed.
                it->seeds.push_back(plant->seedlist.front());

                plant->seedlist.clear();
            }

            // Collect dead matter.
            if (plant->deadBiomass > 0)
            {
                it->Layers.front().nitrates += plant->removedNitrogen;
                it->Layers.front().plantmatter += plant->deadBiomass;

                plant->deadBiomass = plant->removedNitrogen = 0;
            }
        }

        // Run seeds.
        for (Seed& seed : it->seeds)
        {
            if (seed.attemptGerminate(wd))
                it->plants.push_back(BasePlant(seed, &(*it)));
        }

        for (int counter = 0; counter < it->seeds.size(); counter++)
        {
            if (it->seeds.at(counter).germinated)
            {
                it->seeds[counter] = it->seeds.back();
                it->seeds.pop_back();
                counter--;
            }
        }
        
        plantCounter++;
    }

}

int randInt(const int& lower, const int& higher)
{
    // since I'm lazy I'm going to go ahead and use RAND().
    return rand() / RAND_MAX * (higher - lower) + lower + 1;
}

void SoilGrid::addRandomWater(const int& numberOf, const int& howMuch)
{
    for (int counter = numberOf; counter > 0; counter--)
    {
        ref(randInt(0, width - 1), randInt(0, height - 1)).getFront().addWater(howMuch);
    }
}

void SoilGrid::addWaterSquare(const int& x, const int& y, const int& w, const int& h, const double& howMuch)
{
    for (int yC = y; yC < y + h; yC++)
    for (int xC = x; xC < x + w; xC++)
        ref(xC, yC).getFront().addWater(howMuch);
}