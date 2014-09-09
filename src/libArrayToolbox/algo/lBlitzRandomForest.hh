#ifndef LBLITZRANDOMFOREST_H_
#define LBLITZRANDOMFOREST_H_

#include <blitz/array.h>
#include "lRandomForest.hh"

void trainRFSimple(
    lRandomForest &forest, blitz::Array<float,2> &Features,
    blitz::Array<int,1> &Labels, int m_try = -1, int labelRangeLow = 0,
    int labelRangeHigh = 9999);

void saveRFSimple(lRandomForest& forest, std::string fileName);

void loadRFSimple(lRandomForest& forest, std::string fileName, int nTree);

#endif

