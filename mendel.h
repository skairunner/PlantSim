#include <utility>
#include <random>

/**Genes that are quantified as a number are NumberGenes. The returned trait from returnExpressedTrait() is the mean of first and second.**/
struct NumberGene
{
/**The default constructor. The gene ID is set as **/
  NumberGene();
  NumberGene(const double& firstAllele, const double& secondAllele);
  double returnExpressedTrait();
  double first; // Quantify how much it does whatever.
  double second; // ""
};

struct MendelGene
{
  MendelGene();
  MendelGene(const bool& firstAllele, const bool& secondAllele);
  bool returnExpressedTrait();
  bool first; // Dominant or recessive? true is dominant.
  bool second;
};

// Has 4 operator-trait pairs
struct PolyGene
{
public:
  PolyGene();
  PolyGene(const MendelGene& op1, const MendelGene& op2, const NumberGene& firstTrait, const NumberGene& secondTrait);
  double returnExpressedTrait();
  MendelGene operator1; // Controls expression of trait 1. Left or right?
  MendelGene operator2; // "    " of trait 2. Left or right?
  NumberGene trait1;
  NumberGene trait2;
};

class MendelianInheritance
{
public:
  MendelianInheritance();
  MendelianInheritance(const unsigned int& newSeed);
  NumberGene inherit(const NumberGene& left, const NumberGene& right);
  MendelGene inherit(const MendelGene& left, const MendelGene& right);
  // The above two inheritance methods have identical code.
  PolyGene inherit(const PolyGene& left, const PolyGene& right);
  // PolyGene's inheritance method is slightly different, but fundamentally identical.
  virtual void seed(const int& newseed); //Also inheritable.
  PolyGene spawnInRange(const double& min, const double& max); // Spawns a random PolyGene with all NumberGenes in range min to max, and MendelGenes randomly set.
  PolyGene spawnInRange(const std::pair<double, double>& range); // convenience function to use pair<>
  PolyGene spawnInRange(const std::pair<int, int>& range); // convenience function to use pair<>
protected:
  virtual int random(const int& min, const int& max); // Closed interval. 
  virtual double random(const double&min, const double& max);
  std::mt19937 gen;
};

