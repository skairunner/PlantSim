#include "testingSuite.h"
#include "plantDictionary.h"

using namespace ALMANAC;

void Tests::perPlantingDates()
{
    Weather WeatherModule(true);


    fstream batchFile;
    batchFile.open("batchtest.txt", fstream::out | fstream::trunc);

    Month startDate;
    startDate.setMonth(JANUARY);
    startDate.setDate(1);

    batchFile << "Start date\tFinal biomass\tSeed biomass\tNum seeds\n";
    for (int counter = 0; counter < 360; counter++)
    {
        Weather WeatherCopy = WeatherModule;
        WeatherCopy.changeDate(startDate.getMonth(), startDate.getDate());
        SoilGrid soilg(1, 1, 1);
        soilg.ref(0, 0).plants.push_back(BasePlant(&soilg.ref(0, 0)));

        for (int counter2 = 0; counter2 < 250; counter2++)
        {
            WeatherCopy.step();
            soilg.step(WeatherCopy.getDataBundle());
            soilg.stepPlants(WeatherCopy.getDataBundle());
        }

        auto plant = soilg.ref(0, 0).plants.back();
        batchFile << startDate << "\t" << plant.getBiomassStruct() << "\t" << plant.getBiomassStruct().flowerAndfruits << "\t";

        plant.createSeeds(WeatherCopy.getDataBundle().date);
        batchFile << plant.seedlist.size() << "\n";

        cout << counter << " ";

        startDate.advanceDay();
    }

    cout << "Done\n";

    return;
}

void Tests::singlePlant(const int daysToRun, const std::string& plantname, Month startDate)
{
    Weather WeatherModule(true);
    SoilGrid sg(1, 1);


    fstream file;
    file.open("output1.txt", fstream::out | fstream::trunc);
    file << "Date\tHeat units\tHeight(mm)\tRoot depth(mm)\tBiomass(g)\tLAI\twater damage\tPrecipitation\tSurface water\tWater per layer\n";

    fstream Nfile;
    Nfile.open("nitrogen.txt", fstream::out | fstream::trunc);
    Nfile << "Date\tBiomass\tPlant nitrogen\tPrecipitation\tNitrogen\n";

    fstream PlantPartFile;
    PlantPartFile.open("plantparts.txt", fstream::out | fstream::trunc);
    PlantPartFile << "Date\tTemp\tRoot\tStem\tStorage\tFruit\tTotal\tNight length\tFloral induction\n";

    

    //sg.ref(0, 0).requestWater(1000, 0);
    //sg.addWaterSquare(0, 0, 1, 1, 50);


    sg.ref(0, 0).plants.push_back(BasePlant(PD.getPlant(plantname), &sg.ref(0, 0)));
    //sg.ref(0, 0).plants.push_back(BasePlant(&sg.ref(0, 0)));

    

    WeatherModule.changeDate(startDate);
    sg.step(WeatherModule.getDataBundle());

    for (int counter = 0; counter < daysToRun; counter++)
    {
        WeatherModule.step();
        sg.step(WeatherModule.getDataBundle());
        sg.stepPlants(WeatherModule.getDataBundle());


        BasePlant plant = sg.ref(0, 0).plants.back();

        //// output plant stats    
        file << WeatherModule.getMonth() << "\t" << plant.getHU() << "\t" << plant.calcHeight() << "\t" << plant.calcRootDepth() << "\t" << plant.getBiomass() * 1000 << "\t" << plant.getLAI() << "\t" << plant.getWaterREG();

        file << "\t" << WeatherModule.getRainAmount() << "\t" << sg.get(0, 0).surfaceWater;
        for (double val : sg.get(0, 0).inspectWater())
        {
            file << "\t" << val;
        }

        file << "\n";

        //// output nitrogen stats
        Nfile << WeatherModule.getMonth() << "\t" << plant.getBiomass()*1000 << "\t" << plant.getNitrogen() << "\t" << WeatherModule.getRainAmount();
        for (double val : sg.get(0, 0).inspectNitrates())
        {
            Nfile << "\t" << val;
        }

        Nfile << "\n";


        //// output biomass per plant part
        BiomassHolder biomass = plant.getBiomassStruct();
        PlantPartFile << WeatherModule.getMonth() << "\t" << (WeatherModule.getMaxTemp() + WeatherModule.getMinTemp()) / 2.0 << "\t" << biomass.roots
            << "\t" << biomass.stem << "\t" << biomass.storageOrgan << "\t" << biomass.flowerAndfruits << "\t" << biomass << "\t" << WeatherModule.getDataBundle().nightLength
            << "\t" << plant.getInduction() << "\t" << plant.getHU() << "\n";
    }

    sg.ref(0, 0).plants.back().createSeeds(WeatherModule.getMonth());

    auto seeds = sg.ref(0, 0).plants.back().seedlist;

    int seedcounter = 0;
    for (auto seed : seeds)
    {
        seedcounter++;
        cout << "Seed " << seedcounter << " biomass: " << seed.seedBiomass * 1000 << "g\n";
    }



    /////

    cout << sg.ref(0, 0).plants.back().getBiomass();
    if (sg.ref(0, 0).plants.back().isDead())
        cout << "Dead.\n";
    cout << "\nDone.";

    cin.ignore(1);
    return;
}