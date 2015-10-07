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
 * lRandomTree.h
 *
 *  Created on: Jun 11, 2011
 *      Author: liu
 */

#ifndef LRANDOMTREE_H_
#define LRANDOMTREE_H_

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <sstream>
#include <vector>
#include <map>

#include <opencv/cxcore.h>
//#include <opencv/cv.h>

// Auxilary structure
struct IntIndex
{
  float val;
  unsigned int index;
  bool
  operator<(const IntIndex& a) const;
};

// Structure for the leafs
struct LeafNode
{
  int label;
  int numOfInstance;
  int* instanceIndex;
};

// Structure for the normal node
struct Node
{
  Node();
  bool test(const float* f) const;
  float eval(const float* f) const;
  int fIdx;
  float t;
  int left; // Pointer to the left subtree. Negative for leaf node
  int right; // Pointer to the right subtree. Negative for leaf node
};

class lRandomTree
{

public:

  lRandomTree(const char* filename);
  lRandomTree(std::stringstream & in);
  lRandomTree();
  ~lRandomTree();

  // Set/Get functions
  unsigned int GetMaxDepth() const;

  // Classification
  int predict(const float *f) const;

  // Proximity
  void proximity(
      const float *f, int cl, std::map<int, double>& proximityCounter) const;

  // IO functions
  bool saveTree(const char *filename) const;
  bool saveTree(std::stringstream &out) const;

  // Training
  void growTree(
      float **TrainX, int *TrainL, std::vector<int>& TrainSet, int m,
      int n, int maxLabel, float *classWeight, CvRNG *pRNG, int m_try,
      int max_depth, int min_samples = 1, int num_grid = 20);

  int getMaxLabel() const;
  void setMaxLabel(int _maxLabel);

private:

  // Private functions for training
  void grow(const std::vector<int>& TrainSet, int& node, unsigned int depth);

  bool findSplit(
      const std::vector<int>& TrainSet, Node& test,
      std::vector<int> & SetA, std::vector<int>& SetB);

  void generateTest(Node& test);

  void evaluateTest(
      std::vector<IntIndex> & valSet, const Node& test,
      const std::vector<int>& TrainSet);

  void split(
      std::vector<int>& SetA, std::vector<int>& SetB,
      const std::vector<int>& TrainSet, const std::vector<IntIndex>& valSet,
      float t);

  float measureGini(const std::vector<int>& Set, float& pt);

  void makeLeaf(const std::vector<int>& TrainSet, int& node);

  // tree table Data structure
  // 2^(max_depth+1)-1 x colTreeTable matrix as vector
  // column: leafindex f1 f2 b c
  // the test f1 + f2 * b > c
  // if node is not a leaf, leaf=-1
  std::vector<Node*> _treetable;
  int _root;
  std::vector<LeafNode*> _leaves;

  // number of features used for splitting
  unsigned int _m_try;

  // stop growing when number of patches is less than min_samples
  unsigned int _min_samples;

  // depth of the tree: 0-max_depth
  unsigned int _max_depth;

  // number of nodes: 2^(max_depth+1)-1
  unsigned int _num_nodes;

  // number of leafs
  unsigned int _num_leaf;

  // number of iterations for optimizing splitting
  unsigned int _num_grid;

  CvRNG *_cvRNG;

  int _featureDim;

  int _maxLabel;

  // for training
  float** _TrainX;
  int* _TrainL;
  int _nSample;
  float* _classWeight;
};

#endif /* LRANDOMTREE_H_ */
