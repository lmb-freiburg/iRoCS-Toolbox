#include "lRandomForest.hh"

#include <fstream>
#include <cstring>

#include "helper.hh"

lRandomForest::lRandomForest(int nTree)
{
  _nTree = nTree;
  vTrees.resize(nTree);
  classCount = NULL;
  classWeight = NULL;
}

lRandomForest::~lRandomForest()
{
  for (std::vector<lRandomTree*>::iterator it = vTrees.begin();
       it != vTrees.end(); ++it) delete *it;
  vTrees.clear();
  if (classCount != NULL)
      delete classCount;
  if (classWeight != NULL)
      delete classWeight;
}

void lRandomForest::SetTrees(int n)
{
  vTrees.resize(n);
}

int lRandomForest::GetSize() const
{
  return vTrees.size();
}

/******************
 * for simplicity, assuming class labels are 0~maxLabel
 */
int lRandomForest::predict(float* f) const
{
  //    int _maxLabel = vTrees[0]-> getMaxLabel();
  int* _votes = new int[_maxLabel + 1];
  memset(_votes, 0, (_maxLabel + 1) * sizeof(int));
  for (int i = 0; i < _nTree; ++i)
  {
    _votes[vTrees[i]->predict(f)]++;
  }
  int majority = std::max_element(_votes, _votes + _maxLabel + 1) - _votes;
  delete _votes;
  return majority;
}

int lRandomForest::predict(float* f, float& p) const
{
  //    int _maxLabel = vTrees[0]-> getMaxLabel();
  int* _votes = new int[_maxLabel + 1];
  memset(_votes, 0, (_maxLabel + 1) * sizeof(int));
  for (int i = 0; i < _nTree; ++i)
  {
    _votes[vTrees[i]->predict(f)]++;
  }
  int* majority = std::max_element(_votes, _votes + _maxLabel + 1);
  p = float(*majority) / _nTree;
  delete _votes;
  return majority - _votes;
}

int lRandomForest::predict(float* f, float* p) const
{
  //    int _maxLabel = vTrees[0]-> getMaxLabel();
  int* _votes = new int[_maxLabel + 1];
  memset(_votes, 0, (_maxLabel + 1) * sizeof(int));
  for (int i = 0; i < _nTree; ++i)
  {
    _votes[vTrees[i]->predict(f)]++;
  }
  for (int i = 0; i < (_maxLabel + 1); ++i)
  {
    p[i] = float(_votes[i]) / _nTree;
  }
  int* majority = std::max_element(_votes, _votes + _maxLabel + 1);
  delete _votes;
  return majority - _votes;
}

double lRandomForest::proximity(float* f, int cl, int self) const
{
  double prox = 0;
  std::map<int, double> proximityCounter;
  for (int i = 0; i < _nTree; ++i)
  {
    vTrees[i]->proximity(f, cl, proximityCounter);
  }
  if (self != -1)
  {
    for (std::map<int, double>::iterator it = proximityCounter.begin();
         it != proximityCounter.end(); it++)
    {
      prox += (self != it->first) ? it->second * it->second : 0;
    }
  }
  else
  {
    for (std::map<int, double>::iterator it = proximityCounter.begin();
         it != proximityCounter.end(); it++)
    {
      prox += it->second * it->second;
    }
  }
  return prox;
}

double lRandomForest::rawOutlier(float* f, int cl, int self) const
{
  return classCount[cl] / std::max(proximity(f, cl, self), 1.0);
}

double lRandomForest::outlier(float* f, int cl) const
{
  if(classCount[cl] > 0)
      return abs(rawOutlier(f, cl) - medianRawOutlier[cl]) /
          absDevRawOutlier[cl];
  else
      return -1;
}

// Training
/******************
 * for simplicity, assuming class labels are 0~maxLabel
 * max depth is not applied, as RF should use a fully grown tree.
 */
void lRandomForest::trainForest(
    float** X, int* L, int m, int n, int m_try, int max_depth,
    int min_samples, int num_grid)
{
  int max_label = *std::max_element(L, L + m);
  _maxLabel = max_label;
  int* oobVote = new int[m * (max_label + 1)];
  memset(oobVote, 0, (m * (max_label + 1)) * sizeof(int));

  nSample = m;
  classCount = new int[max_label + 1];
  classWeight = new float[max_label + 1];
  memset(classCount, 0, (max_label + 1) * sizeof(int));

  std::cout << "  Training RF on " << m << " samples with " << n << " features"
            << std::endl;

  // here set the class weights as [1 / classPopulation] to balance the
  // training data. Change this according to the problem.
  // loop for growing trees
  for (int i = 0; i < m; ++i) classCount[L[i]]++;

  for (int i = 0; i < max_label + 1; ++i)
      classWeight[i] = classCount[i] > 0 ? (1.0 / classCount[i]) : -1;

#ifdef _OPENMP
#pragma omp parallel for
#endif
  for (int i = 0; i < (int) vTrees.size(); ++i)
  {
    CvRNG cvRNG(i + 1);
    vTrees[i] = new lRandomTree();
    //bootstrap data
    int N = m;
    bool* in = new bool[N];
    std::memset(in, 0, N * sizeof(bool));
    std::vector<int> TrainSet;
    TrainSet.resize(N);
    int sampled = 0;
    for (int j = 0; j < N; ++j)
    {
      sampled = (sampled + cvRandInt(&cvRNG) % N) % N;
//            std::cout << "[" << sampled << "]";
      TrainSet[j] = sampled;
      in[sampled] = true;
    }
//        std::cout << std::endl;
    vTrees[i]->growTree(
        X, L, TrainSet, m, n, max_label, classWeight, &cvRNG, m_try,
        max_depth, min_samples, num_grid);

    //accumulate OOBtest
    for (int j = 0; j < N; ++j)
    {
      if (!in[j])
      {
        int predict = vTrees[i]->predict(X[j]);
        oobVote[j * (max_label + 1) + predict]++;
      }
    }
    delete in;
  }

  //OOBerror

  int* oobErrorCount = new int[max_label + 1];
  int* oobTestCount = new int[max_label + 1];
  memset(oobErrorCount, 0, (max_label + 1) * sizeof(int));
  memset(oobTestCount, 0, (max_label + 1) * sizeof(int));
#ifdef _OPENMP
#pragma omp parallel for
#endif
  for (int i = 0; i < m; ++i)
  {
    int* votes = oobVote + i * (max_label + 1);
    int* majority = std::max_element(votes, votes + (max_label + 1));
    if (*majority > 0)
    {
      int truth = L[i];
      int error = (majority - votes) != truth;
#ifdef _OPENMP
#pragma omp critical
#endif
      {
        oobTestCount[truth]++;
        oobErrorCount[truth] += error;
      }
    }
  }

  std::cout << "Reclassification accuracy for each class" << " ******** "
            << std::endl;
  double oobError = 0;
  double W = 1;
  std::cout << "Label\t#correct\t#total\tAccuracy" << std::endl;
  for (int i = 0; i < max_label + 1; ++i)
  {
    int nCorrect = oobTestCount[i] - oobErrorCount[i];
    if (classWeight[i] > 0)
    {
      std::cout << std::setw(5) << i << "\t"
                << std::setw(8) << nCorrect << "\t"
                << std::setw(6) << oobTestCount[i] << "\t"
                << std::setw(8) << 100 * float(nCorrect) / oobTestCount[i]
                << " %" << std::endl;
      oobError += float(oobErrorCount[i]) / oobTestCount[i];
      W += 1;
    }
  }
  //output
  std::cout << "Overall Accuracy:" << " ******** " << 100.0 * (W - oobError) / W
            << " %" << std::endl;

  delete oobVote;

  //proximity statistics for outlier detection
  double* rawO = new double[m];
#ifdef _OPENMP
#pragma omp parallel for
#endif
  for (int i = 0; i < m; ++i) rawO[i] = rawOutlier(X[i], L[i], i);

  medianRawOutlier = new double[(max_label + 1)];
  absDevRawOutlier = new double[(max_label + 1)];
  memset(medianRawOutlier, 0, (max_label + 1) * sizeof(double));
  memset(absDevRawOutlier, 0, (max_label + 1) * sizeof(double));

  for (int l = 0; l < max_label + 1; ++l)
  {
    if (classWeight[l] > 0)
    {
      std::vector<double> vRaw;
      vRaw.reserve(classCount[l]);
      for (int i = 0; i < m; ++i)
      {
        if (L[i] == l)
            vRaw.push_back(rawO[i]);
      }
      int mid = (int) vRaw.size() / 2;
      std::nth_element(vRaw.begin(), vRaw.begin() + mid, vRaw.end());
      medianRawOutlier[l] = vRaw[mid];
      //        LOG << medianRawOutlier[l] << std::endl;
    }
  }
  for (int i = 0; i < m; ++i)
  {
    absDevRawOutlier[L[i]] +=
        std::min(std::abs(rawO[i] - medianRawOutlier[L[i]]),
                 5 * medianRawOutlier[L[i]]);
  }
  for (int i = 0; i < max_label + 1; ++i)
  {
    if (classWeight[i] > 0)
    {
      absDevRawOutlier[i] /= classCount[i];
    }

    //        LOG << absDevRawOutlier[i] << std::endl;
  }
  delete rawO;
}

// IO functions
void lRandomForest::saveForest(const char* filename, unsigned int offset)
{
  std::ofstream out(filename);
  if (out.is_open())
  {
    out << _nTree << " " << nSample << " " << _maxLabel << std::endl;

    for (int j = 0; j < _maxLabel + 1; j++)
    {
      out << classCount[j] << " " << medianRawOutlier[j] << " "
          << absDevRawOutlier[j] << std::endl;
    }
  }
  //
  char buffer[200];
  for (unsigned int i = 0; i < vTrees.size(); ++i)
  {
    sprintf(buffer, "%s%03d.txt", filename, i + offset);
    vTrees[i]->saveTree(buffer);
  }
}

void lRandomForest::loadForest(const char* filename, unsigned int offset)
{
  std::ifstream in(filename);
  if (in.is_open())
  {
    in >> _nTree;
    in >> nSample;
    in >> _maxLabel;
    classCount = new int[_maxLabel + 1];
    medianRawOutlier = new double[_maxLabel + 1];
    absDevRawOutlier = new double[_maxLabel + 1];
    for (int j = 0; j < _maxLabel + 1; j++)
    {
      in >> classCount[j];
      in >> medianRawOutlier[j];
      in >> absDevRawOutlier[j];
    }
  }
  //
  char buffer[200];
  for (unsigned int i = 0; i < vTrees.size(); ++i)
  {
    sprintf(buffer, "%s%03d.txt", filename, i + offset);
    vTrees[i] = new lRandomTree(buffer);
  }
}

void lRandomForest::saveForest(std::stringstream* & ss)
{
  ss = new std::stringstream[_nTree + 1];
  ss[0] << _nTree << " " << nSample << " " << _maxLabel << std::endl;
  for (int j = 0; j < _maxLabel + 1; j++)
  {
    ss[0] << classCount[j] << " " << medianRawOutlier[j] << " "
          << absDevRawOutlier[j] << std::endl;
  }
  for (unsigned int i = 0; i < vTrees.size(); ++i)
  {
    vTrees[i]->saveTree(ss[i + 1]);
  }
//    LOG << ss[0].str() << std::endl;
}

void lRandomForest::loadForest(std::stringstream*& ss)
{
  ss[0] >> _nTree;
  ss[0] >> nSample;
  ss[0] >> _maxLabel;
  classCount = new int[_maxLabel + 1];
  medianRawOutlier = new double[_maxLabel + 1];
  absDevRawOutlier = new double[_maxLabel + 1];
  for (int j = 0; j < _maxLabel + 1; j++)
  {
    ss[0] >> classCount[j];
    ss[0] >> medianRawOutlier[j];
    ss[0] >> absDevRawOutlier[j];
  }
  //
  for (unsigned int i = 0; i < vTrees.size(); ++i)
  {
    vTrees[i] = new lRandomTree(ss[i + 1]);
  }
}

void lRandomForest::selfTest()
{
  int m = 1000;
  int n = 3;
  float ** X = new float*[m];
  int* L = new int[m];

  CvRNG cvRNG(200);

  for (int i = 0; i < m / 5; i++)
  {
    L[i] = 0;
    float* &x = X[i];
    x = new float[n];
    float aa = cvRandReal(&cvRNG) * 10;
    x[0] = 1 + aa;
    x[1] = 2 + aa;
    x[2] = 3 + aa;
  }

  for (int i = m / 5; i < m / 2; i++)
  {
    L[i] = 1;
    float* &x = X[i];
    x = new float[n];
    float aa = cvRandReal(&cvRNG) * 10;
    x[0] = 3 + aa;
    x[1] = 2 + aa;
    x[2] = 1 + aa;
  }

  for (int i = m / 2; i < m; i++)
  {
    L[i] = 2;
    float* &x = X[i];
    x = new float[n];
    float aa = cvRandReal(&cvRNG) * 10;
    x[0] = 1 + aa;
    x[1] = 3 + aa;
    x[2] = 2 + aa;
  }
  _nTree = 10;
  vTrees.resize(20);
  trainForest(X, L, m, n, 1, 20, 1, 10);

  float p;

  LOG << predict(X[1], p) << std::endl;
  LOG << outlier(X[1], 0) << std::endl;

  saveForest("temTree");
  loadForest("temTree");
  LOG << predict(X[1], p) << std::endl;
  LOG << outlier(X[1], 0) << std::endl;

  lRandomTree loadTree("temTree000.txt");
  loadTree.saveTree("testLoad.txt");

  LOG << predict(X[1], p) << std::endl;
  LOG << outlier(X[1], 0) << std::endl;
  LOG << predict(X[2], p) << std::endl;
  LOG << outlier(X[2], 0) << std::endl;
  //    LOG << p << std::endl;

  LOG << predict(X[300], p) << std::endl;
  LOG << outlier(X[300], 1) << std::endl;
  LOG << predict(X[301], p) << std::endl;
  LOG << outlier(X[301], 1) << std::endl;

  LOG << predict(X[800], p) << std::endl;
  LOG << outlier(X[800], 2) << std::endl;
  LOG << predict(X[900], p) << std::endl;
  LOG << outlier(X[900], 2) << std::endl;

  float a[3] =
      { 2, 3, 4 };

  LOG << predict(a, p) << std::endl;
  LOG << outlier(a, predict(a, p)) << std::endl;
  LOG << p << std::endl;

  float b[3] =
      { 2, 100, 2 };

  LOG << predict(b, p) << std::endl;
  LOG << outlier(b, predict(b, p)) << std::endl;
  LOG << p << std::endl;

  float c[3] =
      { 2, 6, 9 };

  LOG << predict(c, p) << std::endl;
  LOG << outlier(c, predict(c, p)) << std::endl;
  LOG << p << std::endl;
}
