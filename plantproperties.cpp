#include "plantproperties.h"

using namespace ALMANAC;


MendelianInheritance MendelModule;

BiomassHolder::BiomassHolder()
{
    stem = roots = storageOrgan = flowerAndfruits = 0;
}

BiomassHolder::BiomassHolder(double Stem, double Roots, double Storage, double Fruits)
: stem(Stem), roots(Roots), storageOrgan(Storage), flowerAndfruits(Fruits)
{

}

double BiomassHolder::getBiomass() const
{
    return flowerAndfruits + roots + stem + storageOrgan;
}

BiomassHolder::operator double() const
{
    return getBiomass();
}

//////////////
//////////////
//////////////

RGB::RGB()
{
    r = g = b = 0;
}

RGB::RGB(int r1, int g1, int b1)
{
    r = r1;
    g = g1;
    b = b1;
}

//////////////
//////////////
//////////////

PlantProperties MasterPlantProperties::convert(MendelianInheritance& mendel)
{
    PlantProperties out;
    
    out.name = name;

    out.gene_maxLAI = mendel.spawnInRange(maxLAI.first, maxLAI.second);
    out.growthStages = growthStages;
    out.baseTemp = baseTemp;
    out.gene_waterTolerence = mendel.spawnInRange(waterTolerence);
    out.gene_maxHeight = mendel.spawnInRange(maxHeight);
    out.gene_maxYearlyGrowth = mendel.spawnInRange(maxYearlyGrowth);
    out.gene_maxRootDepth = mendel.spawnInRange(maxRootDepth);
    out.HeatUnitFactorNums = HeatUnitFactorNums;
    out.CO2CurveFactors = CO2CurveFactors;
    out.flowerTempCurve = flowerTempCurve;
    out.vernalizationCurve = vernalizationCurve;
    out.LAIGraph = LAIGraph;
    out.nightLengthCurve = nightLengthCurve;

    out.startingNitrogenConcentration = startingNitrogenConcentration;
    out.finalNitrogenConcentration = finalNitrogenConcentration;

    out.baseRatios = baseRatios;
    out.fruitingRatios = fruitingRatios;
    out.finalRatios = finalRatios;

    out.dormantHeightDecrease = dormantHeightDecrease;
    out.dormantRootDecrease = dormantRootDecrease;

    out.minimumTemperature = minimumTemperature;
    out.optimalTemperature = optimalTemperature;

    out.minGerminationTemp = minGerminationTemp;
    out.optimalGerminationTemp = optimalGerminationTemp;
    out.germinationThermalUnits = germinationThermalUnits;
    out.gene_averageFruitWeight = mendel.spawnInRange(averageFruitWeight);
    out.gene_seedRatio = mendel.spawnInRange(seedRatio);
    out.seedViability = seedViability;
    out.dormancy = dormancy;

    out.minFloweringTemp = minFloweringTemp;
    out.optimalFloweringTemp = optimalFloweringTemp;

    out.minVernalizationTemp = minVernalizationTemp;
    out.floralInductionUnitsRequired = floralInductionUnitsRequired;
    out.dayNeutral = dayNeutral;
    out.longDayPlant = longDayPlant;
    out.minimumInduction = minimumInduction;
    out.criticalNightLength = criticalNightLength;

    out.biomassToVPD = biomassToVPD;

    out.isAnnual = isAnnual;
    out.isTree = isTree;
    out.gene_yearsUntilMaturity = mendel.spawnInRange(yearsUntilMaturity);
    out.gene_vegetativeMaturity = mendel.spawnInRange(vegetativeMaturity); // same
    out.gene_maxAge = mendel.spawnInRange(maxAge);
    out.gene_leafFallPeriod = mendel.spawnInRange(leafFallPeriod);

    return out;
}


//////////////
//////////////
//////////////


double PlantProperties::maxLAI()
{
    return gene_maxLAI.returnExpressedTrait();
}


double PlantProperties::waterTolerence()
{
    return gene_waterTolerence.returnExpressedTrait();
}

double PlantProperties::maxYearlyGrowth()
{
    return gene_maxYearlyGrowth.returnExpressedTrait();
}

double PlantProperties::maxRootDepth()
{
    return gene_maxRootDepth.returnExpressedTrait();
}

double PlantProperties::averageFruitWeight()
{
    return gene_averageFruitWeight.returnExpressedTrait();
}

int PlantProperties::yearsUntilMaturity()
{
    return (int)gene_yearsUntilMaturity.returnExpressedTrait();
}

int PlantProperties::vegetativeMaturity()
{
    return (int)gene_vegetativeMaturity.returnExpressedTrait();
}

int PlantProperties::maxAge()
{
    return (int)gene_maxAge.returnExpressedTrait();
}

int PlantProperties::leafFallPeriod()
{
    return (int)gene_leafFallPeriod.returnExpressedTrait();
}

double PlantProperties::maxHeight()
{
    return (int)gene_maxHeight.returnExpressedTrait();
}

double PlantProperties::seedRatio()
{
    return gene_seedRatio.returnExpressedTrait();
}


//////////////
//////////////
//////////////

void PlantVisualProperties::randomizeLerp()
{
    lerp = MendelModule.spawnInRange(0, 1);
}

RGB PlantVisualProperties::getColor()
{
    double l = lerp.returnExpressedTrait();
    int r = int(l * color1.r + (1 - l) * color2.r);
    int g = int(l * color1.g + (1 - l) * color2.g);
    int b = int(l * color1.b + (1 - l) * color2.b);
    return RGB(r, g, b);
}