/**************************************************************************
 *
 * Copyright (C) 2015 Kun Liu, Thorsten Falk
 *
 *        Image Analysis Lab, University of Freiburg, Germany
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
 *
 **************************************************************************/

/*
 * lMorph.hh
 *
 *  Created on: Nov 2, 2010
 *      Author: liu
 */

#ifndef _LMORPH_HH
#define _LMORPH_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <queue>

#include <blitz/array.h>

#include "DisjointSets.hh"

// //-------------------designed for 3D----------------------------------

template<typename VoxelT, int Dim>
void morphDilate(blitz::Array<VoxelT,Dim> &data, int size);

template<typename VoxelT, int Dim>
void morphClose(blitz::Array<VoxelT,Dim> &data, int size);

template<typename MarkerT, typename LabelT, int Dim>
void morphConnectedComponentLabelling(
    blitz::Array<MarkerT,Dim> &marker, blitz::Array<LabelT,Dim> &label,
    int conn);

template<typename VoxelT, typename MarkerT, typename LabelT, int Dim>
void morphWatershed(
    blitz::Array<VoxelT,Dim> &data, blitz::Array<MarkerT,Dim> &marker,
    blitz::Array<LabelT,Dim> &label, int conn);

//with labeled marker
template<typename VoxelT, typename LabelT, int Dim>
void morphWatershed(
    blitz::Array<VoxelT,Dim> &data, blitz::Array<LabelT,Dim> &label, int conn);

/*
 * note that the boundary on the border will be omitted.
 */
template<typename Type, typename bType, int Dim>
void morphBoundaryDetection(
    blitz::Array<Type,Dim> &phi, Type thresh, blitz::Array<bType,Dim> &border);

// helper for watersheding
template<typename DT, typename PT>
struct orderedPrioritizedNode
{
  DT m_value;
  PT m_priority;
  long int m_order;

  explicit orderedPrioritizedNode(DT value, PT priority, long int order);

  //Operator overloading for priority comparison
  bool operator <(orderedPrioritizedNode const &opN) const;
};

static const int moveIn3DNeighbor[26][3] =
{
    { -1, 0, 0 },
    { 1, 0, 0 },
    { 0, -1, 0 },
    { 0, 1, 0 },
    { 0, 0, -1 },
    { 0, 0, 1 },

    { -1, -1, 0 },
    { -1, 1, 0 },
    { 1, -1, 0 },
    { 1, 1, 0 },

    { -1, 0, -1 },
    { -1, 0, 1 },
    { 1, 0, -1 },
    { 1, 0, 1 },

    { 0, -1, -1 },
    { 0, -1, 1 },
    { 0, 1, -1 },
    { 0, 1, 1 },

    { -1, -1, -1 },
    { -1, -1, 1 },
    { -1, 1, -1 },
    { 1, -1, -1 },
    { -1, 1, 1 },
    { 1, -1, 1 },
    { 1, 1, -1 },
    { 1, 1, 1 } };

static const int moveIn3DHalfNeighbor[13][3] =
{
    { -1, 0, 0 },
    { 0, -1, 0 },
    { 0, 0, -1 },

    { -1, -1, 0 },
    { -1, 1, 0 },
    { -1, 0, -1 },
    { -1, 0, 1 },
    { 0, -1, -1 },
    { 0, -1, 1 },

    { -1, -1, -1 },
    { -1, -1, 1 },
    { -1, 1, -1 },
    { -1, 1, 1 }, };

class Walker3D
{
public:
  Walker3D();
  Walker3D(blitz::TinyVector<int, 3> shape, int conn);
  void setLocation(const blitz::TinyVector<int, 3> &p);
  bool getNextNeighbor(blitz::TinyVector<int, 3> &q);
private:
  blitz::TinyVector<int, 3> m_p;
  int m_conn;
  int walks;
  blitz::TinyVector<int, 3> m_shape;
};

class Scanner3D
{
public:
  Scanner3D();
  Scanner3D(blitz::TinyVector<int, 3> shape, int conn);
  void setLocation(const blitz::TinyVector<int, 3> &p);
  bool getNextNeighbor(blitz::TinyVector<int, 3> &q);
private:
  //current position
  blitz::TinyVector<int, 3> m_p;
  // connectivity
  int m_conn;
  //a counter for how many neighbors already visited
  int walks;
  // image size for border check
  blitz::TinyVector<int, 3> m_shape;
};

#include "lmorph.icc"

#endif
