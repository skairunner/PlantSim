#pragma once

namespace ALMANAC
  {
  struct SoilProperties
    {
    SoilProperties(const double& WP, const double& FC, const double& SM, const double& SHC): wiltingPoint(WP), fieldCapacity(FC), saturatedMoisture(SM), SatHydConductivity(SHC){}
    double wiltingPoint, fieldCapacity, saturatedMoisture, //unitless
      SatHydConductivity, // mm/h
      travelTime; // mm/h
    };

  namespace SOIL_MODELS
    {
    class Saxton2006
      {
      friend class SoilModule;
      public:
        SoilProperties setState(const double& sand, const double& clay, const double& organicMatter); // <= 1
        double getWP();
        double getFC();
        double getSatMoisture();
        double getSatHydConductivity();
      private:
        Saxton2006();
        double wiltingPoint, fieldCapacity, saturatedMoisture, SatHydConductivity;
      };

     /// The class SoilModule is the one to be used by soilsim. SoilModule is the interface to various encapsulated soil models.
    class SoilModule
      {
      public:
        void setState(const double& sand, const double& silt, const double& clay, const double& organicMatter);
        SoilProperties getState();
      private:
        Saxton2006 SoilModule1;
        SoilProperties internalBuffer;
      };
    }
  }