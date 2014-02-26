#include "mendel.h"
#include <limits>
#include <stdlib.h>
#include <ctime>

NumberGene::NumberGene()
{
  first = second = 0;
}

NumberGene::NumberGene(const double& firstAllele, const double& secondAllele)
  : first(firstAllele), second(secondAllele)
{}

double NumberGene::returnExpressedTrait()
  {
  return (first + second)/2.0f;
  }

MendelGene::MendelGene()
{
  first = second = false;
}

MendelGene::MendelGene(const bool& firstAllele, const bool& secondAllele)
  : first(firstAllele), second(secondAllele)
{}

bool MendelGene::returnExpressedTrait()
  {
    if (!first && !second) //If both alleles are false, ie recessive.
        return false;
    else return true;
  }

PolyGene::PolyGene()
  {
  }

PolyGene::PolyGene(const MendelGene& op1, const MendelGene& op2, const NumberGene& firstTrait, const NumberGene& secondTrait)
  : operator1(op1), operator2(op2), trait1(firstTrait), trait2(secondTrait)
  {

  }

double PolyGene::returnExpressedTrait()
  {
  // PolyGene inheritance here is intended to have a bell curve/normal distribution of
  // phenotypes. It also aims to prevent converging on the mean in simpler models
  // where genes are always averaged. This is a stochastic model, but this function
  // is a deterministic one.
  // True = trait1, false = trait2.

    return ((operator1.returnExpressedTrait() ? trait1.first : trait2.first)
        + (operator2.returnExpressedTrait() ? trait1.second : trait2.second))
        / 2.0f;
  // Basically, if operator1 is true, take the value of trait1's first. If operator2 is true, take the value of trait1's second.
  // Average and output.
  
  }

MendelianInheritance::MendelianInheritance()
  {
  seed(time(0));
  }

MendelianInheritance::MendelianInheritance(const unsigned int& newSeed)
  {
  seed(newSeed);
  }

void MendelianInheritance::seed(const int& newseed)
{
  srand(newseed);
}

int MendelianInheritance::random(const int& min, const int& max)
{
 std::uniform_int_distribution<> distribution(min,max);
  return distribution(gen);
}

double MendelianInheritance::random(const double& min, const double& max)
{
  std::uniform_real_distribution<double> distribution(min, max);
  return distribution(gen);
}

PolyGene MendelianInheritance::spawnInRange(const double& min, const double& max)
{
    PolyGene out;
    if (min == max)
    {
        out.trait1.first =
            out.trait1.second =
            out.trait2.first =
            out.trait2.second = min;
        out.operator1.first =
            out.operator1.second =
            out.operator2.first =
            out.operator2.second = true;
    }
    else
    {
        // Cache two distributions
        std::uniform_real_distribution<> realdist(min, max);
        std::uniform_int_distribution<> intdist(0, 1);

        out.trait1.first = realdist(gen);
        out.trait1.second = realdist(gen);
        out.trait2.first = realdist(gen);
        out.trait2.second = realdist(gen);
        out.operator1.first = intdist(gen);
        out.operator1.second = intdist(gen);
        out.operator2.first = intdist(gen);
        out.operator2.second = intdist(gen);
    }    

    return out;
}

PolyGene  MendelianInheritance::spawnInRange(const std::pair<double, double>& range)
{
    return spawnInRange(range.first, range.second);
}

PolyGene  MendelianInheritance::spawnInRange(const std::pair<int, int>& range)
{
    return spawnInRange(range.first, range.second);
}

MendelGene MendelianInheritance::inherit(const MendelGene& left, const MendelGene& right)
  {
  MendelGene output;
  output.first = (random(0,1) == 0) ? left.first : right.first; // Randomly pick one.
  output.second = (random(0,1) == 0) ? left.second : right.second; // Randomly pick one.
  return output;
  }

NumberGene MendelianInheritance::inherit(const NumberGene& left, const NumberGene& right)
  {
  NumberGene output;
  output.first = (random(0,1) == 0) ? left.first : right.first; // Randomly pick one.
  output.second = (random(0,1) == 0) ? left.second : right.second; // Randomly pick one.
  return output;
  }

PolyGene MendelianInheritance::inherit(const PolyGene& left, const PolyGene& right)
  {
  PolyGene output;
  output.operator1 = inherit(left.operator1, right.operator1);
  output.operator2 = inherit(left.operator2, right.operator2);
  output.trait1 = inherit(left.trait1, right.trait1);
  output.trait2 = inherit(left.trait2, right.trait2);
  // Another way to do this would be to implement a sort of crossing of trait1 and 2, since they represent identical traits anyways.
  return output;
  }

