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
    
    out = *this;

    out.gene_maxLAI = mendel.spawnInRange(maxLAI_r);

    out.gene_waterTolerence = mendel.spawnInRange(waterTolerence_r);
    out.gene_maxHeight = mendel.spawnInRange(maxHeight_r);
    out.gene_maxYearlyGrowth = mendel.spawnInRange(maxYearlyGrowth_r);
    out.gene_maxRootDepth = mendel.spawnInRange(maxRootDepth_r);

    out.gene_averageFruitWeight = mendel.spawnInRange(averageFruitWeight_r);
    out.gene_seedRatio = mendel.spawnInRange(seedRatio_r);

    out.gene_yearsUntilMaturity = mendel.spawnInRange(yearsUntilMaturity_r);
    out.gene_vegetativeMaturity = mendel.spawnInRange(vegetativeMaturity_r); // same
    out.gene_maxAge = mendel.spawnInRange(maxAge_r);
    out.gene_leafFallPeriod = mendel.spawnInRange(leafFallPeriod_r);

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