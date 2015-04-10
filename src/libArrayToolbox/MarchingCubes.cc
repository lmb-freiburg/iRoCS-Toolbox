/**************************************************************************
 *
 * Copyright (C) 2015 Thorsten Falk
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

#include "MarchingCubes.hh"

#include "ATBLinAlg.hh"
#include "SparseMatrix.hh"

// #define MARCHINGCUBES_DEBUG

#ifdef MARCHINGCUBES_DEBUG
#include "../STLFileWriter.hh"
#endif

namespace atb
{
  
  int MarchingCubes::edgeTable[256] = {
      0x0  , 0x109, 0x203, 0x30a, 0x406, 0x50f, 0x605, 0x70c,
      0x80c, 0x905, 0xa0f, 0xb06, 0xc0a, 0xd03, 0xe09, 0xf00,
      0x190, 0x99 , 0x393, 0x29a, 0x596, 0x49f, 0x795, 0x69c,
      0x99c, 0x895, 0xb9f, 0xa96, 0xd9a, 0xc93, 0xf99, 0xe90,
      0x230, 0x339, 0x33 , 0x13a, 0x636, 0x73f, 0x435, 0x53c,
      0xa3c, 0xb35, 0x83f, 0x936, 0xe3a, 0xf33, 0xc39, 0xd30,
      0x3a0, 0x2a9, 0x1a3, 0xaa , 0x7a6, 0x6af, 0x5a5, 0x4ac,
      0xbac, 0xaa5, 0x9af, 0x8a6, 0xfaa, 0xea3, 0xda9, 0xca0,
      0x460, 0x569, 0x663, 0x76a, 0x66 , 0x16f, 0x265, 0x36c,
      0xc6c, 0xd65, 0xe6f, 0xf66, 0x86a, 0x963, 0xa69, 0xb60,
      0x5f0, 0x4f9, 0x7f3, 0x6fa, 0x1f6, 0xff , 0x3f5, 0x2fc,
      0xdfc, 0xcf5, 0xfff, 0xef6, 0x9fa, 0x8f3, 0xbf9, 0xaf0,
      0x650, 0x759, 0x453, 0x55a, 0x256, 0x35f, 0x55 , 0x15c,
      0xe5c, 0xf55, 0xc5f, 0xd56, 0xa5a, 0xb53, 0x859, 0x950,
      0x7c0, 0x6c9, 0x5c3, 0x4ca, 0x3c6, 0x2cf, 0x1c5, 0xcc ,
      0xfcc, 0xec5, 0xdcf, 0xcc6, 0xbca, 0xac3, 0x9c9, 0x8c0,
      0x8c0, 0x9c9, 0xac3, 0xbca, 0xcc6, 0xdcf, 0xec5, 0xfcc,
      0xcc , 0x1c5, 0x2cf, 0x3c6, 0x4ca, 0x5c3, 0x6c9, 0x7c0,
      0x950, 0x859, 0xb53, 0xa5a, 0xd56, 0xc5f, 0xf55, 0xe5c,
      0x15c, 0x55 , 0x35f, 0x256, 0x55a, 0x453, 0x759, 0x650,
      0xaf0, 0xbf9, 0x8f3, 0x9fa, 0xef6, 0xfff, 0xcf5, 0xdfc,
      0x2fc, 0x3f5, 0xff , 0x1f6, 0x6fa, 0x7f3, 0x4f9, 0x5f0,
      0xb60, 0xa69, 0x963, 0x86a, 0xf66, 0xe6f, 0xd65, 0xc6c,
      0x36c, 0x265, 0x16f, 0x66 , 0x76a, 0x663, 0x569, 0x460,
      0xca0, 0xda9, 0xea3, 0xfaa, 0x8a6, 0x9af, 0xaa5, 0xbac,
      0x4ac, 0x5a5, 0x6af, 0x7a6, 0xaa , 0x1a3, 0x2a9, 0x3a0,
      0xd30, 0xc39, 0xf33, 0xe3a, 0x936, 0x83f, 0xb35, 0xa3c,
      0x53c, 0x435, 0x73f, 0x636, 0x13a, 0x33 , 0x339, 0x230,
      0xe90, 0xf99, 0xc93, 0xd9a, 0xa96, 0xb9f, 0x895, 0x99c,
      0x69c, 0x795, 0x49f, 0x596, 0x29a, 0x393, 0x99 , 0x190,
      0xf00, 0xe09, 0xd03, 0xc0a, 0xb06, 0xa0f, 0x905, 0x80c,
      0x70c, 0x605, 0x50f, 0x406, 0x30a, 0x203, 0x109, 0x0  };

  int MarchingCubes::triangleTable[256][16] = {
      {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {0, 1, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {1, 8, 3, 9, 8, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {0, 8, 3, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {9, 2, 10, 0, 2, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {2, 8, 3, 2, 10, 8, 10, 9, 8, -1, -1, -1, -1, -1, -1, -1},
      {3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {0, 11, 2, 8, 11, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {1, 9, 0, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {1, 11, 2, 1, 9, 11, 9, 8, 11, -1, -1, -1, -1, -1, -1, -1},
      {3, 10, 1, 11, 10, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {0, 10, 1, 0, 8, 10, 8, 11, 10, -1, -1, -1, -1, -1, -1, -1},
      {3, 9, 0, 3, 11, 9, 11, 10, 9, -1, -1, -1, -1, -1, -1, -1},
      {9, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {4, 3, 0, 7, 3, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {0, 1, 9, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {4, 1, 9, 4, 7, 1, 7, 3, 1, -1, -1, -1, -1, -1, -1, -1},
      {1, 2, 10, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {3, 4, 7, 3, 0, 4, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1},
      {9, 2, 10, 9, 0, 2, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1},
      {2, 10, 9, 2, 9, 7, 2, 7, 3, 7, 9, 4, -1, -1, -1, -1},
      {8, 4, 7, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {11, 4, 7, 11, 2, 4, 2, 0, 4, -1, -1, -1, -1, -1, -1, -1},
      {9, 0, 1, 8, 4, 7, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1},
      {4, 7, 11, 9, 4, 11, 9, 11, 2, 9, 2, 1, -1, -1, -1, -1},
      {3, 10, 1, 3, 11, 10, 7, 8, 4, -1, -1, -1, -1, -1, -1, -1},
      {1, 11, 10, 1, 4, 11, 1, 0, 4, 7, 11, 4, -1, -1, -1, -1},
      {4, 7, 8, 9, 0, 11, 9, 11, 10, 11, 0, 3, -1, -1, -1, -1},
      {4, 7, 11, 4, 11, 9, 9, 11, 10, -1, -1, -1, -1, -1, -1, -1},
      {9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {9, 5, 4, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {0, 5, 4, 1, 5, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {8, 5, 4, 8, 3, 5, 3, 1, 5, -1, -1, -1, -1, -1, -1, -1},
      {1, 2, 10, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {3, 0, 8, 1, 2, 10, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1},
      {5, 2, 10, 5, 4, 2, 4, 0, 2, -1, -1, -1, -1, -1, -1, -1},
      {2, 10, 5, 3, 2, 5, 3, 5, 4, 3, 4, 8, -1, -1, -1, -1},
      {9, 5, 4, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {0, 11, 2, 0, 8, 11, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1},
      {0, 5, 4, 0, 1, 5, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1},
      {2, 1, 5, 2, 5, 8, 2, 8, 11, 4, 8, 5, -1, -1, -1, -1},
      {10, 3, 11, 10, 1, 3, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1},
      {4, 9, 5, 0, 8, 1, 8, 10, 1, 8, 11, 10, -1, -1, -1, -1},
      {5, 4, 0, 5, 0, 11, 5, 11, 10, 11, 0, 3, -1, -1, -1, -1},
      {5, 4, 8, 5, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1},
      {9, 7, 8, 5, 7, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {9, 3, 0, 9, 5, 3, 5, 7, 3, -1, -1, -1, -1, -1, -1, -1},
      {0, 7, 8, 0, 1, 7, 1, 5, 7, -1, -1, -1, -1, -1, -1, -1},
      {1, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {9, 7, 8, 9, 5, 7, 10, 1, 2, -1, -1, -1, -1, -1, -1, -1},
      {10, 1, 2, 9, 5, 0, 5, 3, 0, 5, 7, 3, -1, -1, -1, -1},
      {8, 0, 2, 8, 2, 5, 8, 5, 7, 10, 5, 2, -1, -1, -1, -1},
      {2, 10, 5, 2, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1},
      {7, 9, 5, 7, 8, 9, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1},
      {9, 5, 7, 9, 7, 2, 9, 2, 0, 2, 7, 11, -1, -1, -1, -1}, // unchecked
      {2, 3, 11, 0, 1, 8, 1, 7, 8, 1, 5, 7, -1, -1, -1, -1},
      {11, 2, 1, 11, 1, 7, 7, 1, 5, -1, -1, -1, -1, -1, -1, -1},
      {9, 5, 8, 8, 5, 7, 10, 1, 3, 10, 3, 11, -1, -1, -1, -1},
      {5, 7, 0, 5, 0, 9, 7, 11, 0, 1, 0, 10, 11, 10, 0, -1},
      {11, 10, 0, 11, 0, 3, 10, 5, 0, 8, 0, 7, 5, 7, 0, -1},
      {11, 10, 5, 7, 11, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {0, 8, 3, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {9, 0, 1, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {1, 8, 3, 1, 9, 8, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1},
      {1, 6, 5, 2, 6, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {1, 6, 5, 1, 2, 6, 3, 0, 8, -1, -1, -1, -1, -1, -1, -1},
      {9, 6, 5, 9, 0, 6, 0, 2, 6, -1, -1, -1, -1, -1, -1, -1},
      {5, 9, 8, 5, 8, 2, 5, 2, 6, 3, 2, 8, -1, -1, -1, -1},
      {2, 3, 11, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {11, 0, 8, 11, 2, 0, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1},
      {0, 1, 9, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1},
      {5, 10, 6, 1, 9, 2, 9, 11, 2, 9, 8, 11, -1, -1, -1, -1},
      {6, 3, 11, 6, 5, 3, 5, 1, 3, -1, -1, -1, -1, -1, -1, -1},
      {0, 8, 11, 0, 11, 5, 0, 5, 1, 5, 11, 6, -1, -1, -1, -1},
      {3, 11, 6, 0, 3, 6, 0, 6, 5, 0, 5, 9, -1, -1, -1, -1},
      {6, 5, 9, 6, 9, 11, 11, 9, 8, -1, -1, -1, -1, -1, -1, -1},
      {5, 10, 6, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {4, 3, 0, 4, 7, 3, 6, 5, 10, -1, -1, -1, -1, -1, -1, -1},
      {1, 9, 0, 5, 10, 6, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1},
      {10, 6, 5, 1, 9, 7, 1, 7, 3, 7, 9, 4, -1, -1, -1, -1},
      {6, 1, 2, 6, 5, 1, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1},
      {1, 2, 5, 5, 2, 6, 3, 0, 4, 3, 4, 7, -1, -1, -1, -1},
      {8, 4, 7, 9, 0, 5, 0, 6, 5, 0, 2, 6, -1, -1, -1, -1},
      {7, 3, 9, 7, 9, 4, 3, 2, 9, 5, 9, 6, 2, 6, 9, -1},
      {3, 11, 2, 7, 8, 4, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1},
      {5, 10, 6, 4, 7, 2, 4, 2, 0, 2, 7, 11, -1, -1, -1, -1},
      {0, 1, 9, 4, 7, 8, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1},
      {9, 2, 1, 9, 11, 2, 9, 4, 11, 7, 11, 4, 5, 10, 6, -1},
      {8, 4, 7, 3, 11, 5, 3, 5, 1, 5, 11, 6, -1, -1, -1, -1},
      {5, 1, 11, 5, 11, 6, 1, 0, 11, 7, 11, 4, 0, 4, 11, -1},
      {0, 5, 9, 0, 6, 5, 0, 3, 6, 11, 6, 3, 8, 4, 7, -1},
      {6, 5, 9, 6, 9, 11, 4, 7, 9, 7, 11, 9, -1, -1, -1, -1},
      {10, 4, 9, 6, 4, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {4, 10, 6, 4, 9, 10, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1},
      {10, 0, 1, 10, 6, 0, 6, 4, 0, -1, -1, -1, -1, -1, -1, -1},
      {8, 3, 1, 8, 1, 6, 8, 6, 4, 6, 1, 10, -1, -1, -1, -1},
      {1, 4, 9, 1, 2, 4, 2, 6, 4, -1, -1, -1, -1, -1, -1, -1},
      {3, 0, 8, 1, 2, 9, 2, 4, 9, 2, 6, 4, -1, -1, -1, -1},
      {0, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {8, 3, 2, 8, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1},
      {10, 4, 9, 10, 6, 4, 11, 2, 3, -1, -1, -1, -1, -1, -1, -1},
      {0, 8, 2, 2, 8, 11, 4, 9, 10, 4, 10, 6, -1, -1, -1, -1},
      {3, 11, 2, 0, 1, 6, 0, 6, 4, 6, 1, 10, -1, -1, -1, -1},
      {6, 4, 1, 6, 1, 10, 4, 8, 1, 2, 1, 11, 8, 11, 1, -1},
      {9, 6, 4, 9, 3, 6, 9, 1, 3, 11, 6, 3, -1, -1, -1, -1},
      {8, 11, 1, 8, 1, 0, 11, 6, 1, 9, 1, 4, 6, 4, 1, -1},
      {3, 11, 6, 3, 6, 0, 0, 6, 4, -1, -1, -1, -1, -1, -1, -1},
      {6, 4, 8, 11, 6, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {7, 10, 6, 7, 8, 10, 8, 9, 10, -1, -1, -1, -1, -1, -1, -1},
      {0, 7, 3, 0, 10, 7, 0, 9, 10, 6, 7, 10, -1, -1, -1, -1}, // unchecked
      {10, 6, 7, 1, 10, 7, 1, 7, 8, 1, 8, 0, -1, -1, -1, -1},
      {10, 6, 7, 10, 7, 1, 1, 7, 3, -1, -1, -1, -1, -1, -1, -1},
      {1, 2, 6, 1, 6, 8, 1, 8, 9, 8, 6, 7, -1, -1, -1, -1}, //unchecked
      {2, 6, 9, 2, 9, 1, 6, 7, 9, 0, 9, 3, 7, 3, 9, -1},
      {7, 8, 0, 7, 0, 6, 6, 0, 2, -1, -1, -1, -1, -1, -1, -1},
      {7, 3, 2, 6, 7, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {2, 3, 11, 10, 6, 8, 10, 8, 9, 8, 6, 7, -1, -1, -1, -1},
      {2, 0, 7, 2, 7, 11, 0, 9, 7, 6, 7, 10, 9, 10, 7, -1},
      {1, 8, 0, 1, 7, 8, 1, 10, 7, 6, 7, 10, 2, 3, 11, -1},
      {11, 2, 1, 11, 1, 7, 10, 6, 1, 6, 7, 1, -1, -1, -1, -1},
      {8, 9, 6, 8, 6, 7, 9, 1, 6, 11, 6, 3, 1, 3, 6, -1},
      {0, 9, 1, 11, 6, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {7, 8, 0, 7, 0, 6, 3, 11, 0, 11, 6, 0, -1, -1, -1, -1},
      {7, 11, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {3, 0, 8, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {0, 1, 9, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {8, 1, 9, 8, 3, 1, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1},
      {10, 1, 2, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {1, 2, 10, 3, 0, 8, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1},
      {2, 9, 0, 2, 10, 9, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1},
      {6, 11, 7, 2, 10, 3, 10, 8, 3, 10, 9, 8, -1, -1, -1, -1},
      {7, 2, 3, 6, 2, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {7, 0, 8, 7, 6, 0, 6, 2, 0, -1, -1, -1, -1, -1, -1, -1},
      {2, 7, 6, 2, 3, 7, 0, 1, 9, -1, -1, -1, -1, -1, -1, -1},
      {1, 6, 2, 1, 8, 6, 1, 9, 8, 8, 7, 6, -1, -1, -1, -1}, // unchecked
      {10, 7, 6, 10, 1, 7, 1, 3, 7, -1, -1, -1, -1, -1, -1, -1},
      {10, 7, 6, 1, 7, 10, 1, 8, 7, 1, 0, 8, -1, -1, -1, -1},
      {0, 3, 7, 0, 7, 10, 0, 10, 9, 6, 10, 7, -1, -1, -1, -1}, // unchecked
      {7, 6, 10, 7, 10, 8, 8, 10, 9, -1, -1, -1, -1, -1, -1, -1},
      {6, 8, 4, 11, 8, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {3, 6, 11, 3, 0, 6, 0, 4, 6, -1, -1, -1, -1, -1, -1, -1},
      {8, 6, 11, 8, 4, 6, 9, 0, 1, -1, -1, -1, -1, -1, -1, -1},
      {9, 4, 6, 9, 6, 3, 9, 3, 1, 11, 3, 6, -1, -1, -1, -1},
      {6, 8, 4, 6, 11, 8, 2, 10, 1, -1, -1, -1, -1, -1, -1, -1},
      {1, 2, 10, 3, 0, 11, 0, 6, 11, 0, 4, 6, -1, -1, -1, -1},
      {4, 11, 8, 4, 6, 11, 0, 2, 9, 2, 10, 9, -1, -1, -1, -1},
      {10, 9, 3, 10, 3, 2, 9, 4, 3, 11, 3, 6, 4, 6, 3, -1},
      {8, 2, 3, 8, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1},
      {0, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {1, 9, 0, 2, 3, 4, 2, 4, 6, 4, 3, 8, -1, -1, -1, -1},
      {1, 9, 4, 1, 4, 2, 2, 4, 6, -1, -1, -1, -1, -1, -1, -1},
      {8, 1, 3, 8, 6, 1, 8, 4, 6, 6, 10, 1, -1, -1, -1, -1},
      {10, 1, 0, 10, 0, 6, 6, 0, 4, -1, -1, -1, -1, -1, -1, -1},
      {4, 6, 3, 4, 3, 8, 6, 10, 3, 0, 3, 9, 10, 9, 3, -1},
      {10, 9, 4, 6, 10, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {4, 9, 5, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {0, 8, 3, 4, 9, 5, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1},
      {5, 0, 1, 5, 4, 0, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1},
      {11, 7, 6, 8, 3, 4, 3, 5, 4, 3, 1, 5, -1, -1, -1, -1},
      {9, 5, 4, 10, 1, 2, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1},
      {6, 11, 7, 1, 2, 10, 0, 8, 3, 4, 9, 5, -1, -1, -1, -1},
      {7, 6, 11, 5, 4, 10, 4, 2, 10, 4, 0, 2, -1, -1, -1, -1},
      {3, 4, 8, 3, 5, 4, 3, 2, 5, 10, 5, 2, 11, 7, 6, -1},
      {7, 2, 3, 7, 6, 2, 5, 4, 9, -1, -1, -1, -1, -1, -1, -1},
      {9, 5, 4, 0, 8, 6, 0, 6, 2, 6, 8, 7, -1, -1, -1, -1},
      {3, 6, 2, 3, 7, 6, 1, 5, 0, 5, 4, 0, -1, -1, -1, -1},
      {6, 2, 8, 6, 8, 7, 2, 1, 8, 4, 8, 5, 1, 5, 8, -1},
      {9, 5, 4, 10, 1, 6, 1, 7, 6, 1, 3, 7, -1, -1, -1, -1},
      {1, 6, 10, 1, 7, 6, 1, 0, 7, 8, 7, 0, 9, 5, 4, -1},
      {4, 0, 10, 4, 10, 5, 0, 3, 10, 6, 10, 7, 3, 7, 10, -1},
      {7, 6, 10, 7, 10, 8, 5, 4, 10, 4, 8, 10, -1, -1, -1, -1},
      {6, 9, 5, 6, 11, 9, 11, 8, 9, -1, -1, -1, -1, -1, -1, -1},
      {3, 6, 11, 0, 6, 3, 0, 5, 6, 0, 9, 5, -1, -1, -1, -1},
      {0, 11, 8, 0, 5, 11, 0, 1, 5, 5, 6, 11, -1, -1, -1, -1}, // unchecked
      {6, 11, 3, 6, 3, 5, 5, 3, 1, -1, -1, -1, -1, -1, -1, -1},
      {1, 2, 10, 9, 5, 11, 9, 11, 8, 11, 5, 6, -1, -1, -1, -1},
      {0, 11, 3, 0, 6, 11, 0, 9, 6, 5, 6, 9, 1, 2, 10, -1},
      {11, 8, 5, 11, 5, 6, 8, 0, 5, 10, 5, 2, 0, 2, 5, -1},
      {6, 11, 3, 6, 3, 5, 2, 10, 3, 10, 5, 3, -1, -1, -1, -1},
      {5, 8, 9, 5, 2, 8, 5, 6, 2, 3, 8, 2, -1, -1, -1, -1},
      {9, 5, 6, 9, 6, 0, 0, 6, 2, -1, -1, -1, -1, -1, -1, -1},
      {1, 5, 8, 1, 8, 0, 5, 6, 8, 3, 8, 2, 6, 2, 8, -1},
      {1, 5, 6, 2, 1, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {1, 3, 6, 1, 6, 10, 3, 8, 6, 5, 6, 9, 8, 9, 6, -1},
      {10, 1, 0, 10, 0, 6, 9, 5, 0, 5, 6, 0, -1, -1, -1, -1},
      {0, 3, 8, 5, 6, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {10, 5, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {11, 5, 10, 7, 5, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {11, 5, 10, 11, 7, 5, 8, 3, 0, -1, -1, -1, -1, -1, -1, -1},
      {5, 11, 7, 5, 10, 11, 1, 9, 0, -1, -1, -1, -1, -1, -1, -1},
      {10, 7, 5, 10, 11, 7, 9, 8, 1, 8, 3, 1, -1, -1, -1, -1},
      {11, 1, 2, 11, 7, 1, 7, 5, 1, -1, -1, -1, -1, -1, -1, -1},
      {0, 8, 3, 1, 2, 7, 1, 7, 5, 7, 2, 11, -1, -1, -1, -1},
      {9, 7, 5, 9, 2, 7, 9, 0, 2, 2, 11, 7, -1, -1, -1, -1},
      {7, 5, 2, 7, 2, 11, 5, 9, 2, 3, 2, 8, 9, 8, 2, -1},
      {2, 5, 10, 2, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1},
      {8, 2, 0, 8, 5, 2, 8, 7, 5, 10, 2, 5, -1, -1, -1, -1},
      {9, 0, 1, 5, 10, 3, 5, 3, 7, 3, 10, 2, -1, -1, -1, -1},
      {9, 8, 2, 9, 2, 1, 8, 7, 2, 10, 2, 5, 7, 5, 2, -1},
      {1, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {0, 8, 7, 0, 7, 1, 1, 7, 5, -1, -1, -1, -1, -1, -1, -1},
      {9, 0, 3, 9, 3, 5, 5, 3, 7, -1, -1, -1, -1, -1, -1, -1},
      {9, 8, 7, 5, 9, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {5, 8, 4, 5, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1},
      {5, 0, 4, 5, 11, 0, 5, 10, 11, 11, 3, 0, -1, -1, -1, -1},
      {0, 1, 9, 8, 4, 10, 8, 10, 11, 10, 4, 5, -1, -1, -1, -1},
      {10, 11, 4, 10, 4, 5, 11, 3, 4, 9, 4, 1, 3, 1, 4, -1},
      {2, 5, 1, 2, 8, 5, 2, 11, 8, 4, 5, 8, -1, -1, -1, -1},
      {0, 4, 11, 0, 11, 3, 4, 5, 11, 2, 11, 1, 5, 1, 11, -1},
      {0, 2, 5, 0, 5, 9, 2, 11, 5, 4, 5, 8, 11, 8, 5, -1},
      {9, 4, 5, 2, 11, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {2, 5, 10, 3, 5, 2, 3, 4, 5, 3, 8, 4, -1, -1, -1, -1},
      {5, 10, 2, 5, 2, 4, 4, 2, 0, -1, -1, -1, -1, -1, -1, -1},
      {3, 10, 2, 3, 5, 10, 3, 8, 5, 4, 5, 8, 0, 1, 9, -1},
      {5, 10, 2, 5, 2, 4, 1, 9, 2, 9, 4, 2, -1, -1, -1, -1},
      {8, 4, 5, 8, 5, 3, 3, 5, 1, -1, -1, -1, -1, -1, -1, -1},
      {0, 4, 5, 1, 0, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {8, 4, 5, 8, 5, 3, 9, 0, 5, 0, 3, 5, -1, -1, -1, -1},
      {9, 4, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {4, 11, 7, 4, 9, 11, 9, 10, 11, -1, -1, -1, -1, -1, -1, -1},
      {0, 8, 3, 4, 9, 7, 9, 11, 7, 9, 10, 11, -1, -1, -1, -1},
      {1, 10, 11, 1, 11, 4, 1, 4, 0, 7, 4, 11, -1, -1, -1, -1},
      {3, 1, 4, 3, 4, 8, 1, 10, 4, 7, 4, 11, 10, 11, 4, -1},
      {4, 11, 7, 9, 11, 4, 9, 2, 11, 9, 1, 2, -1, -1, -1, -1},
      {9, 7, 4, 9, 11, 7, 9, 1, 11, 2, 11, 1, 0, 8, 3, -1},
      {11, 7, 4, 11, 4, 2, 2, 4, 0, -1, -1, -1, -1, -1, -1, -1},
      {11, 7, 4, 11, 4, 2, 8, 3, 4, 3, 2, 4, -1, -1, -1, -1},
      {2, 9, 10, 2, 7, 9, 2, 3, 7, 7, 4, 9, -1, -1, -1, -1},
      {9, 10, 7, 9, 7, 4, 10, 2, 7, 8, 7, 0, 2, 0, 7, -1},
      {3, 7, 10, 3, 10, 2, 7, 4, 10, 1, 10, 0, 4, 0, 10, -1},
      {1, 10, 2, 8, 7, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {4, 9, 1, 4, 1, 7, 7, 1, 3, -1, -1, -1, -1, -1, -1, -1},
      {4, 9, 1, 4, 1, 7, 0, 8, 1, 8, 7, 1, -1, -1, -1, -1},
      {4, 0, 3, 7, 4, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {4, 8, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {8, 9, 10, 8, 10, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {3, 0, 9, 3, 9, 11, 11, 9, 10, -1, -1, -1, -1, -1, -1, -1},
      {0, 1, 10, 0, 10, 8, 8, 10, 11, -1, -1, -1, -1, -1, -1, -1},
      {3, 1, 10, 11, 3, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {1, 2, 11, 1, 11, 9, 9, 11, 8, -1, -1, -1, -1, -1, -1, -1},
      {3, 0, 9, 3, 9, 11, 1, 2, 9, 2, 11, 9, -1, -1, -1, -1},
      {0, 2, 11, 8, 0, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {3, 2, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {2, 3, 8, 2, 8, 10, 10, 8, 9, -1, -1, -1, -1, -1, -1, -1},
      {9, 10, 2, 0, 9, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {2, 3, 8, 2, 8, 10, 0, 1, 8, 1, 10, 8, -1, -1, -1, -1},
      {1, 10, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {1, 3, 8, 9, 1, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {0, 9, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {0, 3, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}};

  int MarchingCubes::computeTrianglesForGridCell(
      GridCell const &gridCell, std::vector<Triangle>& triangles,
      double isoLevel)
  {
    int cubeIndex = 0;
    for (int i = 0; i < 8; ++i)
        if (gridCell.cornerValues(i) < isoLevel) cubeIndex |= 1 << i;

    // Cube is entirely in/out of the surface
    if (edgeTable[cubeIndex] == 0) return 0;

    // Find the vertices where the surface intersects the cube
    blitz::TinyVector<SurfaceGeometry::VertexT,12> vertexList;
    if (edgeTable[cubeIndex] & 1)
        vertexList(0) =
            getInterpolatedVertex(
                isoLevel, gridCell.cornerCoordinates(0),
                gridCell.cornerCoordinates(1),
                gridCell.cornerValues(0), gridCell.cornerValues(1));
    if (edgeTable[cubeIndex] & 2)
        vertexList(1) =
            getInterpolatedVertex(
                isoLevel, gridCell.cornerCoordinates(1),
                gridCell.cornerCoordinates(2),
                gridCell.cornerValues(1), gridCell.cornerValues(2));
    if (edgeTable[cubeIndex] & 4)
        vertexList(2) =
            getInterpolatedVertex(
                isoLevel, gridCell.cornerCoordinates(2),
                gridCell.cornerCoordinates(3),
                gridCell.cornerValues(2), gridCell.cornerValues(3));
    if (edgeTable[cubeIndex] & 8)
        vertexList(3) =
            getInterpolatedVertex(
                isoLevel, gridCell.cornerCoordinates(3),
                gridCell.cornerCoordinates(0),
                gridCell.cornerValues(3), gridCell.cornerValues(0));
    if (edgeTable[cubeIndex] & 16)
        vertexList(4) =
            getInterpolatedVertex(
                isoLevel, gridCell.cornerCoordinates(4),
                gridCell.cornerCoordinates(5),
                gridCell.cornerValues(4), gridCell.cornerValues(5));
    if (edgeTable[cubeIndex] & 32)
        vertexList(5) =
            getInterpolatedVertex(
                isoLevel, gridCell.cornerCoordinates(5),
                gridCell.cornerCoordinates(6),
                gridCell.cornerValues(5), gridCell.cornerValues(6));
    if (edgeTable[cubeIndex] & 64)
        vertexList(6) =
            getInterpolatedVertex(
                isoLevel, gridCell.cornerCoordinates(6),
                gridCell.cornerCoordinates(7),
                gridCell.cornerValues(6), gridCell.cornerValues(7));
    if (edgeTable[cubeIndex] & 128)
        vertexList(7) =
            getInterpolatedVertex(
                isoLevel, gridCell.cornerCoordinates(7),
                gridCell.cornerCoordinates(4),
                gridCell.cornerValues(7), gridCell.cornerValues(4));
    if (edgeTable[cubeIndex] & 256)
        vertexList(8) =
            getInterpolatedVertex(
                isoLevel, gridCell.cornerCoordinates(0),
                gridCell.cornerCoordinates(4),
                gridCell.cornerValues(0), gridCell.cornerValues(4));
    if (edgeTable[cubeIndex] & 512)
        vertexList(9) =
            getInterpolatedVertex(
                isoLevel, gridCell.cornerCoordinates(1),
                gridCell.cornerCoordinates(5),
                gridCell.cornerValues(1), gridCell.cornerValues(5));
    if (edgeTable[cubeIndex] & 1024)
        vertexList(10) =
            getInterpolatedVertex(
                isoLevel, gridCell.cornerCoordinates(2),
                gridCell.cornerCoordinates(6),
                gridCell.cornerValues(2), gridCell.cornerValues(6));
    if (edgeTable[cubeIndex] & 2048)
        vertexList(11) =
            getInterpolatedVertex(
                isoLevel, gridCell.cornerCoordinates(3),
                gridCell.cornerCoordinates(7),
                gridCell.cornerValues(3), gridCell.cornerValues(7));
            
    int nTriangles = 0;
    for (int i = 0; triangleTable[cubeIndex][i] != -1; i += 3)
    {
      Triangle t(vertexList(triangleTable[cubeIndex][i]),
                 vertexList(triangleTable[cubeIndex][i + 1]),
                 vertexList(triangleTable[cubeIndex][i + 2]));

      // Ensure, that the triangle is not degenerate
      blitz::TinyVector<double,3> n(
          blitz::cross(blitz::TinyVector<double,3>(t(1) - t(0)),
                       blitz::TinyVector<double,3>(t(2) - t(0))));
      if (blitz::dot(n, n) > 0.0)
      {
        triangles.push_back(t);
        nTriangles++;
      }
    }
    // std::cout << std::endl;

    return nTriangles;
  }

  void MarchingCubes::computeTrianglesForGridCell(
      GridCell const &gridCell, blitz::TinyVector<int,12> &cellIndices,
      SurfaceGeometry &surface, double isoLevel)
  {
    std::vector<SurfaceGeometry::VertexT> &V = surface.vertices();
    std::vector<SurfaceGeometry::IndexT> &I = surface.indices();

    int cubeIndex = 0;
    for (int i = 0; i < 8; ++i)
        if (gridCell.cornerValues(i) < isoLevel) cubeIndex |= 1 << i;

    // Cube is entirely in/out of the surface
    if (edgeTable[cubeIndex] == 0) return;

    // Find the vertices where the surface intersects the cube
    blitz::TinyVector<SurfaceGeometry::VertexT,12> vertexList(
        SurfaceGeometry::VertexT(0.0f));
    if ((edgeTable[cubeIndex] & 1) != 0)
    {
      if (cellIndices(0) == -1)
          vertexList(0) = getInterpolatedVertex(
              isoLevel, gridCell.cornerCoordinates(0),
              gridCell.cornerCoordinates(1),
              gridCell.cornerValues(0), gridCell.cornerValues(1));
      else vertexList(0) = V[cellIndices(0)];
    }
    if ((edgeTable[cubeIndex] & 2) != 0)
    {
      if (cellIndices(1) == -1)
          vertexList(1) = getInterpolatedVertex(
              isoLevel, gridCell.cornerCoordinates(1),
              gridCell.cornerCoordinates(2),
              gridCell.cornerValues(1), gridCell.cornerValues(2));
      else vertexList(1) = V[cellIndices(1)];
    }
    if ((edgeTable[cubeIndex] & 4) != 0)
    {
      if (cellIndices(2) == -1)
          vertexList(2) = getInterpolatedVertex(
              isoLevel, gridCell.cornerCoordinates(2),
              gridCell.cornerCoordinates(3),
              gridCell.cornerValues(2), gridCell.cornerValues(3));
      else vertexList(2) = V[cellIndices(2)];
    }
    if ((edgeTable[cubeIndex] & 8) != 0)
    {
      if (cellIndices(3) == -1)
          vertexList(3) = getInterpolatedVertex(
              isoLevel, gridCell.cornerCoordinates(3),
              gridCell.cornerCoordinates(0),
              gridCell.cornerValues(3), gridCell.cornerValues(0));
      else vertexList(3) = V[cellIndices(3)];
    }
    if ((edgeTable[cubeIndex] & 16) != 0)
    {
      if (cellIndices(4) == -1)
          vertexList(4) = getInterpolatedVertex(
              isoLevel, gridCell.cornerCoordinates(4),
              gridCell.cornerCoordinates(5),
              gridCell.cornerValues(4), gridCell.cornerValues(5));
      else vertexList(4) = V[cellIndices(4)];
    }
    if ((edgeTable[cubeIndex] & 32) != 0)
    {
      if (cellIndices(5) == -1)
          vertexList(5) = getInterpolatedVertex(
              isoLevel, gridCell.cornerCoordinates(5),
              gridCell.cornerCoordinates(6),
              gridCell.cornerValues(5), gridCell.cornerValues(6));
      else vertexList(5) = V[cellIndices(5)];
    }
    if ((edgeTable[cubeIndex] & 64) != 0)
    {
      if (cellIndices(6) == -1)
          vertexList(6) = getInterpolatedVertex(
              isoLevel, gridCell.cornerCoordinates(6),
              gridCell.cornerCoordinates(7),
              gridCell.cornerValues(6), gridCell.cornerValues(7));
      else vertexList(6) = V[cellIndices(6)];
    }
    if ((edgeTable[cubeIndex] & 128) != 0)
    {
      if (cellIndices(7) == -1)
          vertexList(7) = getInterpolatedVertex(
              isoLevel, gridCell.cornerCoordinates(7),
              gridCell.cornerCoordinates(4),
              gridCell.cornerValues(7), gridCell.cornerValues(4));
      else vertexList(7) = V[cellIndices(7)];
    }
    if ((edgeTable[cubeIndex] & 256) != 0)
    {
      if (cellIndices(8) == -1)
          vertexList(8) = getInterpolatedVertex(
              isoLevel, gridCell.cornerCoordinates(0),
              gridCell.cornerCoordinates(4),
              gridCell.cornerValues(0), gridCell.cornerValues(4));
      else vertexList(8) = V[cellIndices(8)];
    }
    if ((edgeTable[cubeIndex] & 512) != 0)
    {
      if (cellIndices(9) == -1)
          vertexList(9) = getInterpolatedVertex(
              isoLevel, gridCell.cornerCoordinates(1),
              gridCell.cornerCoordinates(5),
              gridCell.cornerValues(1), gridCell.cornerValues(5));
      else vertexList(9) = V[cellIndices(9)];
    }
    if ((edgeTable[cubeIndex] & 1024) != 0)
    {
      if (cellIndices(10) == -1)
          vertexList(10) = getInterpolatedVertex(
              isoLevel, gridCell.cornerCoordinates(2),
              gridCell.cornerCoordinates(6),
              gridCell.cornerValues(2), gridCell.cornerValues(6));
      else vertexList(10) = V[cellIndices(10)];
    }
    if ((edgeTable[cubeIndex] & 2048) != 0)
    {
      if (cellIndices(11) == -1)
          vertexList(11) = getInterpolatedVertex(
              isoLevel, gridCell.cornerCoordinates(3),
              gridCell.cornerCoordinates(7),
              gridCell.cornerValues(3), gridCell.cornerValues(7));
      else vertexList(11) = V[cellIndices(11)];
    }

    for (int i = 0; triangleTable[cubeIndex][i] != -1; i += 3)
    {
      SurfaceGeometry::NormalT normal(
          blitz::cross(
              SurfaceGeometry::VertexT(
                  vertexList(triangleTable[cubeIndex][i + 1]) -
                  vertexList(triangleTable[cubeIndex][i])),
              SurfaceGeometry::VertexT(
                  vertexList(triangleTable[cubeIndex][i + 2]) -
                  vertexList(triangleTable[cubeIndex][i]))));

      // Ensure, that the triangle is not degenerate
      float n = static_cast<float>(std::sqrt(blitz::dot(normal, normal)));
      if (n > 0.0f)
      {
        for (int j = 0; j < 3; ++j)
        {
          unsigned int corner = triangleTable[cubeIndex][i + j];
          if (cellIndices(corner) == -1)
          {
            V.push_back(vertexList(corner));
            cellIndices(corner) = static_cast<int>(V.size()) - 1;
            for (unsigned int k = corner + 1; k < 12; ++k)
            {
              if (blitz::all(vertexList(corner) == vertexList(k)))
                  cellIndices(k) = cellIndices(corner);
            }
          }
          I.push_back(
              static_cast<SurfaceGeometry::IndexT>(cellIndices(corner)));
        }
      }
    }
  }

  SurfaceGeometry::VertexT MarchingCubes::getInterpolatedVertex(
      double isoLevel, SurfaceGeometry::VertexT const &p1,
      SurfaceGeometry::VertexT const &p2, double val1, double val2)
  {
    if (std::abs(isoLevel - val1) < 0.00001) return p1;
    if (std::abs(isoLevel - val2) < 0.00001) return p2;
    if (std::abs(val1 - val2) < 0.00001) return p1;
    double lambda = (isoLevel - val1) / (val2 - val1);
    return SurfaceGeometry::VertexT(p1 + lambda * (p2 - p1));
  }

  void MarchingCubes::simplifyMesh(
      SurfaceGeometry &surface, double simplifyTolerance)
  {
    std::vector<SurfaceGeometry::VertexT> &V = surface.vertices();
    std::vector<SurfaceGeometry::NormalT> &N = surface.normals();
    std::vector<SurfaceGeometry::IndexT> &I = surface.indices();

    if (V.size() != N.size()) N.resize(V.size());

#ifdef MARCHINGCUBES_DEBUG
    // Some pre-condition assertions
    unsigned int maxIndex = 0;
    if (I.size() % 3 != 0)
    {
      std::cerr << __FILE__ << ":" << __LINE__ << ": Error: Index vector "
                << "length is not dividable by 3. indices.size() = "
                << I.size() << std::endl;
      exit(-1);
    }
    for (size_t i = 0; i < I.size(); ++i) if (I[i] > maxIndex) maxIndex = I[i];
    if (static_cast<size_t>(maxIndex) >= V.size())
    {
      std::cerr << __FILE__ << ":" << __LINE__ << ": Error: Index vector is "
                << "corrupt, it refers to " << maxIndex << " vertices, the "
                << "vertex vector contains only " << V.size()
                << " vertices" << std::endl;
      exit(-1);
    }
#endif

    // Even if no mesh simplification is requested fix the given mesh by
    // removing duplicate vertices and zero area triangles
    _checkAndFixMesh(surface);

    if (simplifyTolerance <= 0.0) return;
    
    // Adjacency matrix
    SparseMatrix<unsigned char> A(V.size(), V.size());

    // Triangle lists for each vertex
    std::vector< std::set<size_t> > T(V.size());

    // Priority queue for edge popping
    typedef std::pair<size_t, size_t> Edge;
    typedef std::multimap<double, Edge> EdgeCostQueue;
    typedef std::pair<double, Edge> EdgeCostQueueEntry;
    EdgeCostQueue C;

    // Matrix containing iterators into the cost queue for fast element removal
    typedef std::map<size_t, EdgeCostQueue::iterator> EdgeVector;
    typedef std::vector< std::map<size_t, EdgeCostQueue::iterator> > EdgeMatrix;
    EdgeMatrix Cp(V.size());
    
    // Initialize adjacency matrix and triangle lists
    for (size_t i = 0; i < I.size(); i += 3)
    {
      A(I[i    ], I[i + 1]) = A(I[i + 1], I[i    ]) = 1;
      A(I[i    ], I[i + 2]) = A(I[i + 2], I[i    ]) = 1;
      A(I[i + 1], I[i + 2]) = A(I[i + 2], I[i + 1]) = 1;
      T[I[i]].insert(i / 3);
      T[I[i + 1]].insert(i / 3);
      T[I[i + 2]].insert(i / 3);
    }

#ifdef MARCHINGCUBES_DEBUG
    // std::cerr << "=== INITIAL STATE ===================" << std::endl;

    // std::cerr << "vertices = " << std::flush;
    // for (std::vector<Vertex>::const_iterator it = vertices.begin();
    //      it != vertices.end(); ++it)
    //     std::cerr << *it << " " << std::flush;
    // std::cerr << std::endl;

    // std::cerr << "indices = " << std::flush;
    // for (std::vector<unsigned int>::const_iterator it =
    //      I.begin(); it != I.end(); ++it)
    //     std::cerr << *it << " " << std::flush;
    // std::cerr << std::endl;

    // {
    //   blitz::Array<short,2> adj(
    //       ptrdiff_t(A.nRows()), ptrdiff_t(A.nColumns()));
    //   adj = blitz::cast<short>(A.toBlitz());
    //   std::cerr << "Adjacency matrix = " << adj << std::endl;
    // }
    
    // std::cerr << "T = ( " << std::flush;
    // for (size_t i = 0; i < T.size(); ++i)
    // {
    //   std::cerr << "{ " << std::flush;
    //   for (std::set<size_t>::const_iterator it = T[i].begin();
    //        it != T[i].end(); ++it)
    //       std::cerr << *it << " " << std::flush;
    //   std::cerr << "} " << std::flush;
    // }
    // std::cerr << ")" << std::endl;
#endif

    // Compute removal costs for each edge
    for (size_t r = 0; r < A.nRows(); ++r)
    {
      for (SparseVector<unsigned char>::const_iterator rowIter =
               A.row(r).begin(); rowIter != A.row(r).end(); ++rowIter)
      {
        if (rowIter->first < r) continue;
        Edge edge(r, rowIter->first);
        double cost = _computeEdgeRemovalCost(edge, surface, T);
        if (cost < simplifyTolerance)
            Cp[r][rowIter->first] = C.insert(EdgeCostQueueEntry(cost, edge));
      }
    }

#ifdef MARCHINGCUBES_DEBUG
    // std::cerr << "Edge cost queue = " << std::flush;
    // for (std::map<double,Edge>::const_iterator it = C.begin(); it != C.end();
    //      ++it)
    // {
    //   std::cerr << "(" << it->first << " -> " << it->second.first << ", "
    //             << it->second.second << ")" << std::flush;
    // }
    // std::cerr << std::endl;

    // std::cerr << "Queued edges = " << std::flush;
    // for (std::map<Edge, std::multimap<double,Edge>::iterator>::const_iterator
    //          it = Cp.begin(); it != Cp.end(); ++it)
    // {
    //   std::cerr << "(" << it->first.first << ", " << it->first.second << ") "
    //             << std::flush;
    // }
    // std::cerr << std::endl;

    // std::cerr << "=====================================" << std::endl;
#endif

    /*---------------------------------------------------------------------
     *  Start the simplification
     *---------------------------------------------------------------------*/ 

#ifdef MARCHINGCUBES_DEBUG
    int iter = 1;
#endif
    while (C.size() != 0)
    {
#ifdef MARCHINGCUBES_DEBUG
      if (iter % 100 == 1)
      {
#ifdef _OPENMP
#pragma omp critical
#endif
        std::cerr << "  Mesh simplification iteration " << iter << " ("
                  << V.size() << " vertices remaining, "
                  << C.size() << " edges in queue)" << std::endl;
      }
      iter++;
#endif

      // Get edge (r, c) to remove
      size_t r = C.begin()->second.first;
      size_t c = C.begin()->second.second;

#ifdef MARCHINGCUBES_DEBUG
      // std::cerr << "Removing edge (" << r << ", " << c << ")" << std::endl;
#endif

      // Remove all edges with distance less than 3 from the removed edge
      for (SparseVector<unsigned char>::const_iterator rowIter =
               A.row(r).begin(); rowIter != A.row(r).end(); ++rowIter)
      {
        for (SparseVector<unsigned char>::const_iterator nbRowIter =
                 A.row(rowIter->first).begin();
             nbRowIter != A.row(rowIter->first).end(); ++nbRowIter)
        {
          Edge e;
          if (nbRowIter->first <= rowIter->first)
              e = Edge(nbRowIter->first, rowIter->first);
          else e = Edge(rowIter->first, nbRowIter->first);
          EdgeVector::iterator it = Cp[e.first].find(e.second);
          if (it != Cp[e.first].end())
          {
            C.erase(it->second);
            Cp[e.first].erase(it);
          }
        }
        Edge e;
        if (rowIter->first <= r) e = Edge(rowIter->first, r);
        else e = Edge(r, rowIter->first);
        EdgeVector::iterator it = Cp[e.first].find(e.second);
        if (it != Cp[e.first].end())
        {
          C.erase(it->second);
          Cp[e.first].erase(it);
        }        
      }
      for (SparseVector<unsigned char>::const_iterator rowIter =
               A.row(c).begin(); rowIter != A.row(c).end(); ++rowIter)
      {
        for (SparseVector<unsigned char>::const_iterator nbRowIter =
                 A.row(rowIter->first).begin();
             nbRowIter != A.row(rowIter->first).end(); ++nbRowIter)
        {
          Edge e;
          if (nbRowIter->first <= rowIter->first)
              e = Edge(nbRowIter->first, rowIter->first);
          else e = Edge(rowIter->first, nbRowIter->first);
          EdgeVector::iterator it = Cp[e.first].find(e.second);
          if (it != Cp[e.first].end())
          {
            C.erase(it->second);
            Cp[e.first].erase(it);
          }
        }
        Edge e;
        if (rowIter->first <= c) e = Edge(rowIter->first, c);
        else e = Edge(c, rowIter->first);
        EdgeVector::iterator it = Cp[e.first].find(e.second);
        if (it != Cp[e.first].end())
        {
          C.erase(it->second);
          Cp[e.first].erase(it);
        }
      }

      // If last vertex is moved, update corresponding queue entries
      if (c < V.size() - 1)
      {
        for (SparseVector<unsigned char>::const_iterator rowIter =
                 A.row(A.nRows() - 1).begin();
             rowIter != A.row(A.nRows() - 1).end(); ++rowIter)
        {
          Edge e;
          e = Edge(rowIter->first, A.nRows() - 1);
          EdgeVector::iterator it = Cp[e.first].find(e.second);
          if (it != Cp[e.first].end())
          {
            Edge eNew;
            if (e.first <= c) eNew = Edge(e.first, c);
            else eNew = Edge(c, e.first);
            it->second->second = eNew;
            Cp[eNew.first][eNew.second] = it->second;
            Cp[e.first].erase(it);
          }
        }
      }
      
#ifdef MARCHINGCUBES_DEBUG
      // std::cerr << "Edge cost queue (after removal) = " << std::flush;
      // for (std::map<double,Edge>::const_iterator it = C.begin(); it != C.end();
      //      ++it)
      // {
      //   std::cerr << "(" << it->first << " -> " << it->second.first << ", "
      //             << it->second.second << ")" << std::flush;
      // }
      // std::cerr << std::endl;
      
      // std::cerr << "Queued edges = " << std::flush;
      // for (std::map<Edge, std::multimap<double,Edge>::iterator>::const_iterator
      //          it = Cp.begin(); it != Cp.end(); ++it)
      // {
      //   std::cerr << "(" << it->first.first << ", " << it->first.second << ") "
      //             << std::flush;
      // }
      // std::cerr << std::endl;
#endif

      // Update vertex list
      V[r] = 0.5 * (V[r] + V[c]);
      if (c < V.size() - 1) V[c] = V[V.size() - 1];
      V.pop_back();
      
#ifdef MARCHINGCUBES_DEBUG
      // std::cerr << "vertices = " << std::flush;
      // for (std::vector<SurfaceGeometry::VertexT>::const_iterator
      //      it = V.begin(); it != V.end(); ++it)
      //     std::cerr << *it << " " << std::flush;
      // std::cerr << std::endl;
#endif

      // In index list replace all occurrences of c by r and all occurrences
      // of vertices.size() by c, for this check all triangles around those
      // vertices
      for (std::set<size_t>::const_iterator itT = T[c].begin();
           itT != T[c].end(); ++itT)
      {
        for (int d = 0; d < 3; ++d)
            if (static_cast<size_t>(I[3 * *itT + d]) == c)
                I[3 * *itT + d] = static_cast<unsigned int>(r);
      }
      if (c < V.size())
      {
        for (std::set<size_t>::const_iterator itT = T[V.size()].begin();
             itT != T[V.size()].end(); ++itT)
        {
          for (int d = 0; d < 3; ++d)
              if (static_cast<size_t>(I[3 * *itT + d]) == V.size())
                  I[3 * *itT + d] = static_cast<unsigned int>(c);
        }
      }

#ifdef MARCHINGCUBES_DEBUG
      // std::cerr << "In the following all occurrences of " << c
      //           << " should be replaced by " << r
      //           << " and all occurrences of " << vertices.size()
      //           << " should be replaced by " << c << std::endl;
      // std::cerr << "indices = " << std::flush;
      // for (std::vector<SurfaceGeometry::IndexT>::const_iterator it =
      //      I.begin(); it != I.end(); ++it)
      //     std::cerr << *it << " " << std::flush;
      // std::cerr << std::endl;
#endif

      // Update adjacency matrix
      for (SparseVector<unsigned char>::const_iterator rowIter =
               A.row(c).begin(); rowIter != A.row(c).end(); ++rowIter)
      {
        if (rowIter->first != r)
        {
          A(r, rowIter->first) = rowIter->second;
          A(rowIter->first, r) = rowIter->second;
        }
      }
      A.clearRow(c);
      A.clearColumn(c);
      for (SparseVector<unsigned char>::const_iterator rowIter =
               A.row(A.nRows() - 1).begin();
           rowIter != A.row(A.nRows() - 1).end(); ++rowIter)
      {
        A(c, rowIter->first) = rowIter->second;
        A(rowIter->first, c) = rowIter->second;
      }
      A.resize(V.size(), V.size());

      // emptyTriangles = intersection of T[r] and T[c]
      // T[r] = (union of T[r] and T[c]) - emptyTriangles
      std::vector<size_t> emptyTriangles;
      for (std::set<size_t>::const_iterator itC = T[c].begin();
           itC != T[c].end(); ++itC)
      {
        std::set<size_t>::iterator itR = T[r].find(*itC);
        if (itR != T[r].end())
        {
          emptyTriangles.push_back(*itR);
          T[r].erase(itR);
        }
        else T[r].insert(*itC);
      }

      // Move triangle list of last vertex to c
      if (c < V.size()) T[c] = T.back();

      // Remove triangle list of moved vertex
      T.pop_back();

      // Remove empty triangles from triangle lists of all vertices adjacent
      // to 3
      for (SparseVector<unsigned char>::const_iterator rowIter =
               A.row(r).begin(); rowIter != A.row(r).end(); ++rowIter)
      {
        for (size_t i = 0; i < emptyTriangles.size(); ++i)
            T[rowIter->first].erase(emptyTriangles[i]);
      }

      // Replace indices of empty triangles by indices of last triangles
      std::set<size_t> verticesForTriangleUpdate;
      std::map<size_t,size_t> triangleShiftMap;
      for (size_t i = 0; i < emptyTriangles.size(); ++i)
      {
        if (I[3 * emptyTriangles[i]] < I.size() - 3)
        {
          triangleShiftMap[(I.size() - 1) / 3] = emptyTriangles[i];
          I[3 * emptyTriangles[i]] = I[I.size() - 3];
          verticesForTriangleUpdate.insert(I[3 * emptyTriangles[i]]);
          I[3 * emptyTriangles[i] + 1] = I[I.size() - 2];
          verticesForTriangleUpdate.insert(I[3 * emptyTriangles[i] + 1]);
          I[3 * emptyTriangles[i] + 2] = I[I.size() - 1];
          verticesForTriangleUpdate.insert(I[3 * emptyTriangles[i] + 2]);
          for (size_t j = i + 1; j < emptyTriangles.size(); ++j)
          {
            if (3 * emptyTriangles[j] == I.size() - 3)
            {
              emptyTriangles[j] = i;
              break;
            }
          }
        }
        I.pop_back();
        I.pop_back();
        I.pop_back();
      }

      // Check whether verticesForTriangleUpdate contains removed indices
      std::set<size_t> verticesToUpdate;
      for (std::set<size_t>::const_iterator it =
               verticesForTriangleUpdate.begin();
           it != verticesForTriangleUpdate.end(); ++it)
          if (*it < V.size()) verticesToUpdate.insert(*it);

      // Update triangle lists to reflect new triangle ordering
      for (std::set<size_t>::const_iterator itV = verticesToUpdate.begin();
           itV != verticesToUpdate.end(); ++itV)
      {
        for (std::map<size_t,size_t>::const_iterator itM =
                 triangleShiftMap.begin(); itM != triangleShiftMap.end(); ++itM)
        {
          std::set<size_t>::iterator itT = T[*itV].find(itM->first);
          if (itT != T[*itV].end())
          {
            T[*itV].erase(itT);
            T[*itV].insert(itM->second);
          }
        }
      }
      
      /*-----------------------------------------------------------------
       *  Update edge removal cost queue
       *-----------------------------------------------------------------*/
      for (SparseVector<unsigned char>::const_iterator rowIter =
               A.row(r).begin(); rowIter != A.row(r).end(); ++rowIter)
      {
        for (SparseVector<unsigned char>::const_iterator nbRowIter =
                 A.row(rowIter->first).begin();
             nbRowIter != A.row(rowIter->first).end(); ++nbRowIter)
        {
          if (nbRowIter->first < rowIter->first) continue;
          Edge edge(rowIter->first, nbRowIter->first);
          double cost = _computeEdgeRemovalCost(edge, surface, T);
          if (cost < simplifyTolerance)
              Cp[edge.first][edge.second] =
                  C.insert(EdgeCostQueueEntry(cost, edge));
        }
        if (rowIter->first < r) continue;
        Edge edge(r, rowIter->first);
        double cost = _computeEdgeRemovalCost(edge, surface, T);
        if (cost < simplifyTolerance)
            Cp[edge.first][edge.second] =
                C.insert(EdgeCostQueueEntry(cost, edge));
      }
    }
  }

  double MarchingCubes::_computeEdgeRemovalCost(
      std::pair<size_t,size_t> const &edge,
      SurfaceGeometry const &surface,
      std::vector< std::set<size_t> > const &triangles)
  { 
    std::vector<SurfaceGeometry::VertexT> const &V = surface.vertices();
    std::vector<SurfaceGeometry::IndexT> const &I = surface.indices();    

    SurfaceGeometry::VertexT newVertex =
        0.5 * (V[edge.first] + V[edge.second]);
    double errorVolume = 0.0;
    
    // Sum up volumes of left end tetrahedrons
    for (std::set<size_t>::const_iterator trIt =
             triangles[edge.first].begin();
         trIt != triangles[edge.first].end(); ++trIt)
    {
      // Ignore edge adjacent triangles (They will be added when
      // computing the volume of the right end tetrahedrons)
      if (I[3 * *trIt] == edge.second ||
          I[3 * *trIt + 1] == edge.second ||
          I[3 * *trIt + 2] == edge.second) continue;
      blitz::TinyMatrix<double,4,4> tetra;
      tetra = V[I[3 * *trIt]](0),
          V[I[3 * *trIt + 1]](0),
          V[I[3 * *trIt + 2]](0),
          newVertex(0),
          V[I[3 * *trIt]](1),
          V[I[3 * *trIt + 1]](1),
          V[I[3 * *trIt + 2]](1),
          newVertex(1),
          V[I[3 * *trIt]](2),
          V[I[3 * *trIt + 1]](2),
          V[I[3 * *trIt + 2]](2),
          newVertex(2),
          1.0, 1.0, 1.0, 1.0;
      errorVolume += std::abs(determinant(tetra));
    }
    
    // Add the volumes of the right end tetrahedrons
    for (std::set<size_t>::const_iterator trIt =
             triangles[edge.second].begin();
         trIt != triangles[edge.second].end(); ++trIt)
    {
      blitz::TinyMatrix<double,4,4> tetra;
      tetra = V[I[3 * *trIt]](0),
          V[I[3 * *trIt + 1]](0),
          V[I[3 * *trIt + 2]](0),
          newVertex(0),
          V[I[3 * *trIt]](1),
          V[I[3 * *trIt + 1]](1),
          V[I[3 * *trIt + 2]](1),
          newVertex(1),
          V[I[3 * *trIt]](2),
          V[I[3 * *trIt + 1]](2),
          V[I[3 * *trIt + 2]](2),
          newVertex(2),
          1.0, 1.0, 1.0, 1.0;
      errorVolume += std::abs(determinant(tetra));
    }
    return errorVolume;
  }

  void MarchingCubes::_checkAndFixMesh(SurfaceGeometry &surface)
  {
    std::vector<SurfaceGeometry::VertexT> &V = surface.vertices();
    std::vector<SurfaceGeometry::NormalT> &N = surface.normals();
    std::vector<SurfaceGeometry::IndexT> &I = surface.indices();    

    // Find duplicate vertices
    std::vector<size_t> indexMap(V.size());
    std::vector<SurfaceGeometry::VertexT> oldVertices(V);
    std::vector<SurfaceGeometry::NormalT> oldNormals(N);
    V.clear();
    N.clear();
    for (size_t i = 0; i < oldVertices.size(); ++i)
    {
      size_t j;
      for (j = 0;
           j < V.size() &&
               blitz::dot(oldVertices[i] - V[j], oldVertices[i] - V[j]) >
               1.0e-20; ++j);
      if (j == V.size())
      {
        V.push_back(oldVertices[i]);
        if (i < oldNormals.size()) N.push_back(oldNormals[i]);
      }
      indexMap[i] = j;
    }

    // Update index list
    for (size_t i = 0; i < I.size(); ++i)
        I[i] = static_cast<unsigned int>(indexMap[I[i]]);

    // Check for triangles referencing the same vertex multiple times
    // And remove them
    for (size_t i = 0; i < I.size(); i += 3)
    {
      if (I[i] == I[i + 1] || I[i] == I[i + 2] || I[i + 1] == I[i + 2])
      {
        if (i < I.size() - 3)
        {
          I[i] = I[I.size() - 3];
          I[i + 1] = I[I.size() - 2];
          I[i + 2] = I[I.size() - 1];
        }
        I.pop_back();
        I.pop_back();
        I.pop_back();
      }
    }
  }

}
