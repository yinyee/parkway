// ### Coarsener.cpp ###
//
// Copyright (C) 2004, Aleksandar Trifunovic, Imperial College London
//
// HISTORY:
//
// 4/1/2005: Last Modified
//
// ###
#include "coarseners/serial/coarsener.hpp"
#include "hypergraph/serial/hypergraph.hpp"
#include "data_structures/dynamic_array.hpp"

namespace parkway {
namespace serial {
namespace ds = parkway::data_structures;

coarsener::coarsener(int minimum_number_of_nodes, int max_weight, double ratio)
    :  parkway::base::coarsener(max_weight, minimum_number_of_nodes, ratio) {
}

coarsener::~coarsener() {}

hypergraph *coarsener::build_coarse_hypergraph(dynamic_array<int> coarseWts,
                                               int numCoarseVerts,
                                               int totWt) const {

  int numNewHedges = 0;
  int numNewPins = 0;
  int startOff;
  int newHedgeLen;
  int duplDeg;
  int startV;
  int numDupls;
  int hEdge;
  int duplHedge;
  int startHedgeOffset;
  int endHedgeOffset;
  int hEdgeStart;
  int hEdgeEnd;
  int v;

  int i;
  int j;
  int ij;

  serial::hypergraph *newHypergraph = new serial::hypergraph(coarseWts, numCoarseVerts);

  ds::dynamic_array<int> newHedgeOffsets(1024);
  ds::dynamic_array<int> newPinList(1024);
  ds::dynamic_array<int> newHedgeWt(1024);
  ds::dynamic_array<int> newVerOffsets(1024);
  ds::dynamic_array<int> newVtoHedges(1024);

  ds::dynamic_array<int> tempPinList(numPins);
  ds::dynamic_array<int> vDegs(numCoarseVerts);
  ds::dynamic_array<int> duplDegs(numCoarseVerts);
  ds::dynamic_array<int> vHedgOffsets(numCoarseVerts + 1);
  ds::dynamic_array<int> vHedges;

  for (i = 0; i < numCoarseVerts; ++i) {
    vDegs[i] = 0;
    duplDegs[i] = 0;
  }

  // ###
  // here there are two schemes for hyperedge contraction
  // one runs in |E|h^2/2 time
  // other in |E|h(2+log h)
  // pick latter for now
  // ###

  for (i = 0; i < numHedges; ++i) {
    endHedgeOffset = hEdgeOffsets[i + 1];
    startHedgeOffset = hEdgeOffsets[i];

    for (j = startHedgeOffset; j < endHedgeOffset; ++j)
      tempPinList[j] = matchVector[pinList[j]];

    tempPinList.sort_between(startHedgeOffset, endHedgeOffset - 1);

    ++vDegs[tempPinList[startHedgeOffset]];
  }

  vHedgOffsets[0] = 0;

  for (i = 1; i <= numCoarseVerts; ++i)
    vHedgOffsets[i] = vHedgOffsets[i - 1] + vDegs[i - 1];

  vHedges.resize(vHedgOffsets[numCoarseVerts]);

  for (i = 0; i < numCoarseVerts; ++i)
    vDegs[i] = 0;

  // ###
  // build the new pin list
  // ###

  newHedgeOffsets[numNewHedges] = numNewPins;
  numDupls = 0;

  for (i = 0; i < numHedges; ++i) {
    endHedgeOffset = hEdgeOffsets[i + 1];
    startHedgeOffset = numNewPins;

    for (j = hEdgeOffsets[i]; j < endHedgeOffset; ++j) {
      v = tempPinList[j];

      if (newHedgeOffsets[numNewHedges] == numNewPins ||
          v != newPinList[numNewPins - 1]) {
        newPinList[numNewPins++] = v;
      }
    }

    newHedgeLen = numNewPins - newHedgeOffsets[numNewHedges];

    if (newHedgeLen > 1) {
      startV = newPinList[startHedgeOffset];
      duplHedge = -1;
      duplDeg = duplDegs[startV];
      startOff = vHedgOffsets[startV];

      for (j = 0; j < duplDeg; ++j) {
        hEdge = vHedges[startOff + j];
        hEdgeEnd = newHedgeOffsets[hEdge + 1];
        hEdgeStart = newHedgeOffsets[hEdge];

        if (hEdgeEnd - hEdgeStart == newHedgeLen) {
          duplHedge = hEdge;
          for (ij = 1; ij < newHedgeLen; ++ij)
            if (newPinList[hEdgeStart + ij] !=
                newPinList[startHedgeOffset + ij]) {
              duplHedge = -1;
              break;
            }

          if (duplHedge == hEdge)
            break;
        }
      }

      if (duplHedge == -1) {
        vHedges[vHedgOffsets[startV] + duplDeg] = numNewHedges;
        ++duplDegs[startV];

        for (j = newHedgeOffsets[numNewHedges]; j < numNewPins; ++j) {
          v = newPinList[j];
          ++vDegs[v];
        }

        newHedgeWt[numNewHedges++] = hEdgeWeight[i];
        newHedgeOffsets[numNewHedges] = numNewPins;
      } else {
        ++numDupls;
        newHedgeWt[duplHedge] += hEdgeWeight[i];
        numNewPins = newHedgeOffsets[numNewHedges];
      }
    } else
      numNewPins = newHedgeOffsets[numNewHedges];
  }

  // ###
  // build the new vToHedges
  // ###

  newVerOffsets[0] = 0;

  for (i = 1; i < numCoarseVerts + 1; ++i) {
    ij = i - 1;
    newVerOffsets[i] = newVerOffsets[ij] + vDegs[ij];
    vDegs[ij] = 0;
  }

#ifdef PRUDENT
  assert(newVerOffsets[numCoarseVerts] == numNewPins);
#endif

  newVtoHedges.resize(numNewPins);

  for (i = 0; i < numNewHedges; ++i) {
    endHedgeOffset = newHedgeOffsets[i + 1];

    for (j = newHedgeOffsets[i]; j < endHedgeOffset; ++j) {
      v = newPinList[j];
      newVtoHedges[newVerOffsets[v] + (vDegs[v]++)] = i;
    }
  }

  // ###
  // init new hypergraph
  // ###

  newHedgeWt.resize(numNewHedges);
  newPinList.resize(numNewPins);
  newHedgeOffsets.resize(numNewHedges + 1);

  newHypergraph->set_number_of_hyperedges(numNewHedges);
  newHypergraph->set_number_of_pins(numNewPins);
  newHypergraph->set_total_weight(totWt);
  newHypergraph->set_hyperedge_weights(newHedgeWt);
  newHypergraph->set_pin_list(newPinList);
  newHypergraph->set_hyperedge_offsets(newHedgeOffsets);
  newHypergraph->set_vertex_to_hyperedges(newVtoHedges);
  newHypergraph->set_vertex_offsets(newVerOffsets);

  return newHypergraph;
}

}  // namespace serial
}  // namespace parkway
