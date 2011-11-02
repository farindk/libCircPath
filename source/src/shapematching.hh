#ifndef __SHAPEMATCHING__
#define __SHAPEMATCHING__

#include "path.hh"

enum DTWMode {
  VERTEX_BASED, 
  EDGE_BASED	  
};


template <class CostType, DTWMode mode>
class ShapeMatching{};


template <class CostType>
class ShapeMatching<CostType,VERTEX_BASED>
{
public:
  void setDissimilarityMatrix(int n, CostType *dissimilarity);
  inline CostType getMatchingScore() {return (CostType) matching.cost;};
  inline Path getMatching()          {return matching;};

private:
  Path matching;
};


template <class CostType>
class ShapeMatching<CostType,EDGE_BASED>
{
public:
  void setDissimilarityMatrix(int n, CostType *dissimilarity);
  inline CostType getMatchingScore() {return (CostType) matching.cost;};
  inline Path getMatching()          {return matching;};

private:
  Path matching;
};

#include "shapematching.inl"
#endif

