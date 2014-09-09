#include "lRandomTree.hh"

#include <iostream>
#include <fstream>
#include <set>
#include <cstring>

bool IntIndex::operator<(const IntIndex& a) const
{
  return val < a.val;
}


Node::Node()
{
  t = 0;
  fIdx = -1;
}

bool Node::test(const float* f) const
{
  return f[fIdx] > t;
}

float Node::eval(const float* f) const
{
  return f[fIdx];
}


lRandomTree::lRandomTree(const char* filename)
{
  std::cout << "Load Tree " << filename << std::endl;

  int dummy;

  std::ifstream in(filename);
  if (in.is_open())
  {
    // allocate memory for tree table
    //        in >> _max_depth;
    in >> _num_nodes;
    //        _num_nodes = (int) (pow(2.0, int(_max_depth + 1))) - 1;
    _treetable.resize(_num_nodes);
    in >> _num_leaf;
    _leaves.resize(_num_leaf);
    //        leaf = new LeafNode[num_leaf];
    in >> _featureDim;
    in >> _maxLabel;
    in >> _root;
    // read tree nodes
    for (unsigned int n = 0; n < _num_nodes; ++n)
    {
      Node* ptT = new Node;
      in >> dummy;
      in >> ptT->fIdx;
      in >> ptT->t;
      in >> ptT->left;
      in >> ptT->right;
      _treetable[n] = ptT;
    }
    for (unsigned int n = 0; n < _num_leaf; ++n)
    {
      LeafNode *ptL = new LeafNode;
      in >> dummy;
      in >> ptL->label;
      int num;
      in >> num;
      ptL->numOfInstance = num;
      ptL->instanceIndex = new int[num];
      for (int j = 0; j < num; j++)
      {
        in >> ptL->instanceIndex[j];
      }
      _leaves[n] = ptL;
    }
  }
  else
  {
    std::cerr << "Could not read tree: " << filename << std::endl;
  }
  in.close();
}

lRandomTree::lRandomTree(std::stringstream & in)
{
  int dummy;
  // allocate memory for tree table
  //        in >> _max_depth;
  in >> _num_nodes;
  //        _num_nodes = (int) (pow(2.0, int(_max_depth + 1))) - 1;
  _treetable.resize(_num_nodes);
  in >> _num_leaf;
  _leaves.resize(_num_leaf);
  //        leaf = new LeafNode[num_leaf];
  in >> _featureDim;
  in >> _maxLabel;
  in >> _root;
  // read tree nodes
  for (unsigned int n = 0; n < _num_nodes; ++n)
  {
    Node* ptT = new Node;
    in >> dummy;
    in >> ptT->fIdx;
    in >> ptT->t;
    in >> ptT->left;
    in >> ptT->right;
    _treetable[n] = ptT;
  }
  for (unsigned int n = 0; n < _num_leaf; ++n)
  {
    LeafNode *ptL = new LeafNode;
    in >> dummy;
    in >> ptL->label;
    int num;
    in >> num;
    ptL->numOfInstance = num;
    ptL->instanceIndex = new int[num];
    for (int j = 0; j < num; j++)
    {
      in >> ptL->instanceIndex[j];
    }
    _leaves[n] = ptL;
  }
}

lRandomTree::lRandomTree()
{}

lRandomTree::~lRandomTree()
{
  for (unsigned int n = 0; n < _num_nodes; ++n)
  {
    delete _treetable[n];
  }
  for (unsigned int n = 0; n < _num_leaf; ++n)
  {
    delete _leaves[n];
  }
}

// Set/Get functions
unsigned int lRandomTree::GetMaxDepth() const
{
  return _max_depth;
}

// Classification
int lRandomTree::predict(const float *f) const
{
  // pointer to current node
  int pnode = _root;
  // Go through tree until one arrives at a leaf, i.e. pnode[0]>=0)
  while (pnode >= 0)
  {
    bool test = _treetable[pnode]->test(f);
    pnode = test ? _treetable[pnode]->right : _treetable[pnode]->left;
  }
  // return leaf
  return _leaves[-pnode - 1]->label;
}

// Proximity
void lRandomTree::proximity(
    const float *f, int cl, std::map<int, double>& proximityCounter) const
{
  // pointer to current node
  int pnode = _root;
  // Go through tree until one arrives at a leaf, i.e. pnode[0]>=0)
  while (pnode >= 0)
  {
    bool test = _treetable[pnode]->test(f);
    pnode = test ? _treetable[pnode]->right : _treetable[pnode]->left;
  }
  // return leaf

  LeafNode* ptL = _leaves[-pnode - 1];
  if (ptL->label != cl)
      return;
  //    double invPop = 1.0 / ptL->numOfInstance;
  for (int j = 0; j < ptL->numOfInstance; j++)
  {
    std::map<int, double>::iterator re = proximityCounter.find(
        ptL->instanceIndex[j]);
    if (re == proximityCounter.end())
    {
      proximityCounter.insert(
          std::pair<int, double>(ptL->instanceIndex[j], 1.0));
    }
    else
    {
      (re->second) += 1.0;
    }
  }
  return;
}

// IO functions
bool lRandomTree::saveTree(const char *filename) const
{
  std::cout << "Save Tree " << filename << std::endl;
  bool done = false;
  std::ofstream out(filename);
  if (out.is_open())
  {
    out << _num_nodes << " " << _num_leaf << " " << _featureDim << " "
        << _maxLabel << " " << _root << std::endl;
    // save tree nodes
    for (unsigned int n = 0; n < _num_nodes; ++n)
    {
      Node *ptT = _treetable[n];
      out << n << " " << ptT->fIdx << " " << ptT->t << " " << ptT->left
          << " " << ptT->right << std::endl;
    }
    for (unsigned int n = 0; n < _num_leaf; ++n)
    {
      LeafNode *ptL = _leaves[n];
      int num = ptL->numOfInstance;
      out << n << " " << ptL->label << " " << num << std::endl;
      for (int j = 0; j < num; j++)
      {
        out << ptL->instanceIndex[j] << " ";
      }
      out << std::endl;
    }
    out.close();
    done = true;
  }

  return done;
}

bool lRandomTree::saveTree(std::stringstream & out) const
{
  bool done = false;

  out << _num_nodes << " " << _num_leaf << " " << _featureDim << " "
      << _maxLabel << " " << _root << std::endl;
  // save tree nodes
  for (unsigned int n = 0; n < _num_nodes; ++n)
  {
    Node *ptT = _treetable[n];
    out << n << " " << ptT->fIdx << " " << ptT->t << " " << ptT->left
        << " " << ptT->right << std::endl;
  }
  for (unsigned int n = 0; n < _num_leaf; ++n)
  {
    LeafNode *ptL = _leaves[n];
    int num = ptL->numOfInstance;
    out << n << " " << ptL->label << " " << num << std::endl;
    for (int j = 0; j < num; j++)
    {
      out << ptL->instanceIndex[j] << " ";
    }
    out << std::endl;
  }
  done = true;
  return done;
}

// Training
void lRandomTree::growTree(
    float **TrainX, int *TrainL, std::vector<int>& TrainSet, int m,
    int n, int maxLabel, float *classWeight, CvRNG *pRNG, int m_try,
    int max_depth, int min_samples, int num_grid)
{
  _featureDim = n;
  _nSample = m;
  _maxLabel = maxLabel;
  _classWeight = classWeight;
  _m_try = m_try;
  _max_depth = max_depth;
  _min_samples = min_samples;

  _num_nodes = 0;
  _num_leaf = 0;

  _TrainX = TrainX;
  _TrainL = TrainL;
  _num_grid = num_grid;
  _cvRNG = pRNG;
// #pragma omp critical
//     {
//       LOG << TrainX[0][0] << std::endl;
//       LOG << "_maxLabel" << _maxLabel << std::endl;
//       LOG << "_classWeight[0~3]" << _classWeight[0] << " " << _classWeight[1]
//           << " " << _classWeight[2] << " " << _classWeight[3] << " " << std::endl;
//     }
  _root = -1;
  grow(TrainSet, _root, 0);
}

int lRandomTree::getMaxLabel() const
{
  return _maxLabel;
}

void lRandomTree::setMaxLabel(int _maxLabel)
{
  this->_maxLabel = _maxLabel;
}

void lRandomTree::grow(
    const std::vector<int>& TrainSet, int& node, unsigned int depth)
{
// #pragma omp critical
//    std::cerr << "from node " << node << "*************************** at depth "
//         << depth << std::endl;
  //debug
  //    for (std::vector<int>::const_iterator it = TrainSet.begin(); it
  //        != TrainSet.end(); it++)
  //      {
  //        std::cerr << "[" << *it << "," << _TrainL[*it] << "]";
  //      }
  //    std::cerr << std::endl;
  bool oneClass = true;
  std::set<int> labels;
  for (std::vector<int>::const_iterator it = TrainSet.begin(); it
           != TrainSet.end(); it++)
  {
    labels.insert(_TrainL[*it]);
    if (labels.size() > 1)
    {
      oneClass = false;
      break;
    }
  }
//    std::cerr << std::endl;

// #ifdef _OPENMP
// #pragma omp critical
// #endif
//           {
//             if (oneClass)
//                 std::cerr << "Current sub-tree only contains one class"
//                           << std::endl;
//             if (TrainSet.size() <= _min_samples)
//                 std::cerr << "Current subtree contains too few samples for a "
//                           << "split: numSamples = " << TrainSet.size() << ", "
//                           << _min_samples << " needed." << std::endl;
//           }
          
  if (!oneClass && TrainSet.size() > _min_samples)
  {
    std::vector<int> SetA;
    std::vector<int> SetB;
    Node test;

    // Find optimal test
    if (findSplit(TrainSet, test, SetA, SetB))
    {
      node = _num_nodes++;
      // Store binary test for current node
      if (node >= int(_treetable.size()))
      {
        _treetable.resize(node + 1);
      }
      Node* ptT = new Node();
      _treetable[node] = ptT;
      ptT->t = test.t;
      ptT->fIdx = test.fIdx;
//            std::cerr << "Final_Split A/B " << SetA.size() << " " << SetB.size()
//                << std::endl;

      // Go left
      // If enough patches are left continue growing else stop
      if (SetA.size() > _min_samples)
      {
        ptT->left = node;
        grow(SetA, ptT->left, depth + 1);
      }
      else
      {
        ptT->left = node;
        makeLeaf(SetA, ptT->left);
      }

      // Go right
      // If enough patches are left continue growing else stop
      if (SetB.size() > _min_samples)
      {
        ptT->right = node;
        grow(SetB, ptT->right, depth + 1);
      }
      else
      {
        ptT->right = node;
        makeLeaf(SetB, ptT->right);
      }
    }
    else
    {
      // Could not find split (only invalid one leave split)
      makeLeaf(TrainSet, node);
    }
  }
  else
  {
    // maximum depth is reached or only one-class left
    makeLeaf(TrainSet, node);
  }
}

bool lRandomTree::findSplit(
    const std::vector<int>& TrainSet, Node& test,
    std::vector<int> & SetA, std::vector<int>& SetB)
{
  bool found = false;

  // temporary data for split into Set A and Set B
  std::vector<int> tmpA;
  std::vector<int> tmpB;

  // temporary data for finding best test
  std::vector<IntIndex> valSet(TrainSet.size());

  // maximize!!!!
  double bestMeasure = -DBL_MAX;
  Node tmpTest;

  float pt = 0;
  float gini = measureGini(TrainSet, pt);

//    LOG << "gini " << gini << "pt " << pt << std::endl;
  // Find best test of ITER iterations
  for (unsigned int i = 0; i < _m_try; ++i)
  {
    // reset temporary data for split
    tmpA.clear();
    tmpB.clear();

    // generate binary test without threshold
    generateTest(tmpTest);

    //        LOG << "test " << i << ":" << tmpTest.fIdx << std::endl;

    // compute value for each feature
    evaluateTest(valSet, tmpTest, TrainSet);

    // find min/max values for threshold
    float vmin = valSet.front().val;
    float vmax = valSet.back().val;
    float d = vmax - vmin;

    if (d > 0)
    {
      // Find best threshold
      for (unsigned int j = 0; j < _num_grid; ++j)
      {
        // Generate some random thresholds
        float tr = float(j + cvRandReal(_cvRNG)) / _num_grid * d + vmin;

        //                std::cerr << "tr " << j << ":" << tr << "\t";

        split(tmpA, tmpB, TrainSet, valSet, tr);
        // Do not allow empty set split (all patches end up in set A or B)
        if (tmpA.size() > 0 && tmpB.size() > 0)
        {
          // Measure quality of split with classification
          //    The Gini splitting criterion is the decrease of impurity defined as
          //    ∆i ( s, t ) = i (t ) − p L i (t L ) − pR i (t R ) ,
          float ptA, ptB;
          float giniA = measureGini(tmpA, ptA);
          float giniB = measureGini(tmpB, ptB);
          float tmpMeasure = gini - (ptA * giniA + ptB * giniB) / pt;
          //                    std::cerr << "[A " << giniA << "-" << ptA << " B " << giniB
          //                        << "-" << ptB << " infoGain " << tmpMeasure << "]\t";
          // Take binary test with best split
          if (tmpMeasure > bestMeasure)
          {
            found = true;
            bestMeasure = tmpMeasure;
            test.fIdx = tmpTest.fIdx;
            test.t = tr;
            //                                              std::cerr << "bestDist: " << bestDist << " " << tmpA.size() << " " << tmpB.size() << std::endl;
          }
        }
      } // end for j
//            std::cerr << std::endl;
    }
  } // end iter

  if (found)
  {
    // Split training data into two sets A,B accroding to threshold t
    evaluateTest(valSet, test, TrainSet);
    split(SetA, SetB, TrainSet, valSet, test.t);
  }
  // return true if a valid test has been found
  // test is invalid if only splits with an empty set A or B has been created
  return found;
}

void lRandomTree::generateTest(Node& test)
{
  test.fIdx = cvRandInt(_cvRNG) % _featureDim;
}

void lRandomTree::evaluateTest(
    std::vector<IntIndex> & valSet, const Node& test,
    const std::vector<int>& TrainSet)
{
  valSet.resize(TrainSet.size());
  for (unsigned int i = 0; i < TrainSet.size(); ++i)
  {
    valSet[i].val = test.eval(_TrainX[TrainSet[i]]);
    valSet[i].index = i;
  }
  sort(valSet.begin(), valSet.end());
}

void lRandomTree::split(
    std::vector<int>& SetA, std::vector<int>& SetB,
    const std::vector<int>& TrainSet, const std::vector<IntIndex>& valSet,
    float t)
{
  // search largest value such that val<t
  std::vector<IntIndex>::const_iterator it = valSet.begin();
  while (it != valSet.end() && it->val < t)
  {
    ++it;
  }
  SetA.resize(it - valSet.begin());
  SetB.resize(TrainSet.size() - SetA.size());

  it = valSet.begin();
  for (unsigned int i = 0; i < SetA.size(); ++i, ++it)
  {
    SetA[i] = TrainSet[it->index];
  }

  it = valSet.begin() + SetA.size();
  for (unsigned int i = 0; i < SetB.size(); ++i, ++it)
  {
    SetB[i] = TrainSet[it->index];
  }
}

float lRandomTree::measureGini(const std::vector<int>& Set, float& pt)
{
  //The Gini impurity measure at a node t is defined as
  //          i (t ) = ∑i , j C (i | j ) p (i | t ) p ( j | t ) .
  //  where
  //          p ( j | t ) , j = 1, ..., J The probability of a case in class j given that it falls into node t.
  //          C (i | j ) The cost of miss-classifying a class j case as a class i case. Clearly C( j | j) = 0 .

  /*************************************
   * use map
   */
  /*
    map<int, float> labelCounter;
    for (std::vector<int>::const_iterator it = Set.begin(); it != Set.end(); it++)
    {
    map<int, float>::iterator find = labelCounter.find(_TrainL[*it]);
    if (find != labelCounter.end())
    {
    (find->second)++;
    }
    else
    {
    labelCounter.insert(pair<int, float> (_TrainL[*it], 1.0f));
    }
    }
    int NS = Set.size();
    float gini = 1;
    float p;
    for (map<int, float>::iterator it = labelCounter.begin(); it
    != labelCounter.end(); it++)
    {
    p = it->second / NS;
    gini -= p * p;
    }
  */
  int* labelCounter = new int[(_maxLabel + 1)];
  std::memset(labelCounter, 0, (_maxLabel + 1) * sizeof(int));
  for (std::vector<int>::const_iterator it = Set.begin(); it != Set.end(); it++)
  {
    labelCounter[_TrainL[*it]]++;
  }
  //    int NS = Set.size();
  float gini = 1;
  //    float* p = new float[(_maxLabel+1)];
  float p;
  float sumP = 0;
  float sumsqrP = 0;
  for (int i = 0; i < (_maxLabel + 1); i++)
  {
    if (_classWeight[i] > 0)
    {
      p = labelCounter[i] * _classWeight[i];
      sumP += p;
      sumsqrP += p * p;
    }
  }
  gini = 1 - sumsqrP / (sumP * sumP);
  delete labelCounter;
  //    delete p;
  pt = sumP;
  return gini;
}

void lRandomTree::makeLeaf(const std::vector<int>& TrainSet, int& node)
{
  /**********************
   * use map
   */
  /*
    map<int, int> labelCounter;
    for (std::vector<int>::const_iterator it = TrainSet.begin(); it
    != TrainSet.end(); it++)
    {
    map<int, int>::iterator find = labelCounter.find(_TrainL[*it]);
    if (find != labelCounter.end())
    {
    (find->second)++;
    }
    else
    {
    labelCounter.insert(pair<int, int> (_TrainL[*it], 1));
    }
    }

    int maxLabel = -1, maxCount = 0;
    for (map<int, int>::iterator it = labelCounter.begin(); it
    != labelCounter.end(); it++)
    {
    if (it->second > maxCount)
    {
    maxCount = it->second;
    maxLabel = it->first;
    }
    }*/

  int* labelCounter = new int[(_maxLabel + 1)];
  std::memset(labelCounter, 0, (_maxLabel + 1) * sizeof(int));
  std::set<int> Set;
  for (std::vector<int>::const_iterator it = TrainSet.begin(); it
           != TrainSet.end(); it++)
  {
    labelCounter[_TrainL[*it]]++;
    Set.insert(*it);
  }

  int maxLabel =
      std::max_element(labelCounter, labelCounter + (_maxLabel + 1)) -
      labelCounter;

  int lnode = _num_leaf;
  if (lnode >= int(_leaves.size()))
  {
    _leaves.resize(lnode + 1);
  }
  LeafNode* ptL = new LeafNode;
  _leaves[lnode] = ptL;
  ptL->label = maxLabel;
  int num = (int) (Set.size());
  ptL->numOfInstance = num;
  ptL->instanceIndex = new int[num];
  copy(Set.begin(), Set.end(), ptL->instanceIndex);

//    std::cerr << "Leaf Node" << lnode << " with " << TrainSet.size() << " samples. "
//        << std::endl;

  _num_leaf++;

  node = -lnode - 1;
  delete labelCounter;
}
