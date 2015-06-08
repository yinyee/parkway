
#ifndef _KWAY_GREEDY_REFINER_HPP
#define _KWAY_GREEDY_REFINER_HPP

// ### GreedyKwayRefiner.hpp ###
//
// Copyright (C) 2004, Aleksandar Trifunovic, Imperial College London
//
// HISTORY:
//
// 25/4/2004: Last Modified
//
// ###

#include "Refiner.hpp"
#include "VertexNode.hpp"

using namespace std;

class GreedyKwayRefiner : public Refiner {

protected:
  int numNonPosMoves;
  double limit;

  // ###
  // data structures from point of view of vertices
  // ###

  DynamicArray<int> numNeighParts;
  DynamicArray<int> neighboursOfV;
  DynamicArray<int> neighboursOfVOffsets;

  // ###
  // data structures from point of view of hyperedges
  // ###

  DynamicArray<int> hEdgeVinPart;
  DynamicArray<int> hEdgeVinPartOffsets;

  // ###
  // auxiliary structures
  // ###

  DynamicArray<int> vertices;
  DynamicArray<int> vertSeen;
  DynamicArray<int> seenVertices;
  DynamicArray<int> partsSpanned;

public:
  GreedyKwayRefiner(int max, int nparts, double ave, double limit, int dL);
  ~GreedyKwayRefiner();

  void dispRefinerOptions(ostream &out) const;
  void buildDataStructs();
  void destroyDataStructs();
  int initDataStructs();
  void updateAdjVertStat(int v, int sP, int bestDP);

  void refine(Hypergraph &h);
  void rebalance(Hypergraph &h);

  int runGreedyPass();
  int runRebalancingPass();

  inline void setLimit() {
    double a = static_cast<double>(numVertices) * limit;
    numNonPosMoves = static_cast<int>(floor(a));
  }

  inline int findHeaviestOverweight() const {
    int i = 0;
    int p = -1;

    for (; i < numParts; ++i) {
      if (partWeights[i] > maxPartWt) {
        if (p == -1)
          p = i;
        else if (partWeights[i] > partWeights[p])
          p = i;
      }
    }

    return p;
  }
};

#endif
