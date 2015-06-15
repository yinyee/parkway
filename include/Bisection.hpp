
#ifndef _BISECTION_HPP
#define _BISECTION_HPP

// ### Bisection.hpp ###
//
// Copyright (C) 2004, Aleksandar Trifunovic, Imperial College London
//
// HISTORY:
//
// 30/11/2004: Last Modified
//
// ###

#include "hypergraph/serial/hypergraph.hpp"

namespace serial = parkway::serial;

class bisection {

protected:
  int bisect_again_;
  int number_of_vertices_;
  int part_id_;

  serial::hypergraph *hypergraph_;

  dynamic_array<int> map_to_orig_vertices_;

public:
  bisection(serial::hypergraph *h, int bAgain, int pID) {
    hypergraph_ = h;

    number_of_vertices_ = h->number_of_vertices();
    bisect_again_ = bAgain;
    part_id_ = pID;
  }

  inline void initMap() {
    int i;

    map_to_orig_vertices_.reserve(number_of_vertices_);

    for (i = 0; i < number_of_vertices_; ++i)
      map_to_orig_vertices_[i] = i;
  }

  inline void setMap(int *vMap, int nV) {
    map_to_orig_vertices_.set_data(vMap, nV);
  }

  inline int part_id() const { return part_id_; }
  inline int bisect_again() const { return bisect_again_; }
  inline int number_of_vertices() const { return number_of_vertices_; }
  inline int *map_to_orig_vertices() const { return map_to_orig_vertices_.data(); }

  inline serial::hypergraph *hypergraph() const { return hypergraph_; }
};

#endif
