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
