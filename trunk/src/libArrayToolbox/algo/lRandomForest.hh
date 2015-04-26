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
 * lRandomForest.h
 *
 *  Created on: Jun 11, 2011
 *      Author: liu
 */

#ifndef LRANDOMFOREST_H_
#define LRANDOMFOREST_H_

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include<vector>
#include<sstream>

#include "lRandomTree.hh"

class lRandomForest
{

public:

  lRandomForest(int nTree);
  ~lRandomForest();

  void SetTrees(int n);
  int GetSize() const;

  /******************
   * for simplicity, assuming class labels are 0~maxLabel
   */
  int predict(float* f) const;
  int predict(float* f, float& p) const;
  int predict(float* f, float* p) const;

  double proximity(float* f, int cl, int self = -1) const;

  double rawOutlier(float* f, int cl, int self = -1) const;

  double outlier(float* f, int cl) const;

  // Training
  /******************
   * for simplicity, assuming class labels are 0~maxLabel
   * max depth is not applied, as RF should use a fully grown tree.
   */
  void trainForest(
      float** X, int* L, int m, int n, int m_try, int max_depth,
      int min_samples = 1, int num_grid = 20);

  // IO functions
  void saveForest(const char* filename, unsigned int offset = 0);
  void loadForest(const char* filename, unsigned int offset = 0);
  void saveForest(std::stringstream* & ss);
  void loadForest(std::stringstream*& ss);
  void selfTest();

  // Data
  std::vector<lRandomTree*> vTrees;
  int _nTree;
  int nSample;
  int _maxLabel;

  int* classCount;
  float* classWeight;

  double* medianRawOutlier;
  double* absDevRawOutlier;
};

#endif /* LRANDOMFOREST_H_ */
