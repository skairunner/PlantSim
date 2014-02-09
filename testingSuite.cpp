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

    std::string filename = "logs/output_";
    filename += plantname + ".txt";
    fstream file;
    file.open(filename.c_str(), fstream::out | fstream::trunc);
    file << "Date\tHeat units\tHeight(mm)\tRoot depth(mm)\tBiomass(g)\tLAI\tREG\tPrecipitation\tSurface water\tWater per layer\tSnow\n";

    fstream Nfile;
    filename = "logs/nitrogen_";
    filename += plantname + ".txt";
    Nfile.open(filename.c_str(), fstream::out | fstream::trunc);
    Nfile << "Date\tBiomass\tPlant nitrogen\tPrecipitation\tNitrogen\n";

    fstream PlantPartFile;
    filename = "logs/plantparts_";
    filename += plantname + ".txt";
    PlantPartFile.open(filename.c_str(), fstream::out | fstream::trunc);
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
        file << WeatherModule.getMonth() << "\t" << plant.getHU() << "\t" << plant.calcHeight() << "\t" << plant.calcRootDepth() << "\t" << plant.getBiomass() * 1000 << "\t" << plant.getLAI() << "\t" << plant.getREG();

        file << "\t" << WeatherModule.getRainAmount() << "\t" << sg.get(0, 0).surfaceWater;
        for (double val : sg.get(0, 0).inspectWater())
        {
            file << "\t" << val;
        }

        file << "\t" << sg.ref(0, 0).snow << "\n";

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

void Tests::multiplePlants(const int daysToRun, const std::vector<std::string>& plantnames, Month startDate)
{
    Weather WeatherModule(true);
    SoilGrid sg(1, 1);

    string filename = "";
    for (string s : plantnames)
        filename += s + "_";
    filename += ".txt";

    for (int counter = 0; counter < plantnames.size()-1; counter++)
        sg.ref(0, 0).plants.push_back(BasePlant(PD.getPlant(plantnames[0]), &sg.ref(0, 0)));


    fstream rad; rad.open("logs/rad", fstream::trunc | fstream::out);
    fstream LAIlog; LAIlog.open("logs/LAIlog-" + filename, fstream::trunc | fstream::out);
    fstream Nlog; Nlog.open("logs/nitrogen-" + filename, fstream::trunc | fstream::out);

    fstream logs; logs.open(("logs/log-" + filename).c_str(), fstream::trunc | fstream::out);
    logs << "Date\t";
    for (int counter = 0; counter < plantnames.size(); counter++)
    {
        logs << "Height" << counter << "\t"
            << "Biomass" << counter << "\t"
            << "LAI" << counter << "\t";
        rad << "Plant" << counter << " rad\t";
    }
    rad << "Used rad\tTotal rad\n";
    logs << "\n";

    LAIlog << "LAI\tREG\n";

    Nlog << "Date";
    auto soilLayers = sg.ref(0, 0).getLayers();
    int counter = 0;
    for (auto it : soilLayers)
        Nlog << "\tNitrogen" << counter++;
    Nlog << "\n";

    WeatherModule.changeDate(startDate);
    sg.step(WeatherModule.getDataBundle());

    for (int counter = 0; counter < daysToRun; counter++)
    {
        if (counter == 360 * 0)
        {
            if (plantnames.size() > 2)
                sg.ref(0, 0).plants.push_back(BasePlant(PD.getPlant(plantnames.back()), &sg.ref(0, 0)));
        }
            
        WeatherModule.step();
        sg.step(WeatherModule.getDataBundle());
        sg.stepPlants(WeatherModule.getDataBundle());

        for (int c = 0; c < sg.radPerPlant.size(); c++)
        {
            rad << sg.radPerPlant[c] << "\t";
        }

        if (sg.radPerPlant.size() < plantnames.size())
        for (int c = plantnames.size() - sg.radPerPlant.size(); c > 0; c--)
            rad << "\t";
        rad << sg.test_totalrad << "\t" << WeatherModule.getDayRadiation() << "\n";

        logs << WeatherModule.getMonth() << "\t";
        //// output plant stats    
        for (auto p : sg.ref(0, 0).plants)
        {
            logs << p.calcHeight() << "\t" << p.getBiomass() * 1000 << "\t" << p.getLAI() << "\t";
        }
        logs << "\n";

        if (sg.ref(0, 0).plants.size() == plantnames.size())
            LAIlog << sg.ref(0, 0).plants.back().getLAI() << "\t" << sg.ref(0, 0).plants.back().getREG() << "\n";
        else
            LAIlog << "\t\n";

        // nitrogen stats
        soilLayers = sg.ref(0, 0).getLayers();
        Nlog << WeatherModule.getDataBundle().date;
        for (auto it : soilLayers)
            Nlog << "\t" << it.nitrates;
        Nlog << "\n";
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