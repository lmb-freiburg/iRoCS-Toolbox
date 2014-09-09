/**************************************************************************
**       Title: 
**    $RCSfile$
**   $Revision: 4820 $$Name$
**       $Date: 2011-11-08 10:57:01 +0100 (Tue, 08 Nov 2011) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.3  2005/10/26 06:58:54  ronneber
**  - added prettyPrintConfusionTable()
**
**  Revision 1.2  2004/09/08 14:18:03  ronneber
**  - removed unused variable
**
**  Revision 1.1  2004/08/26 08:36:58  ronneber
**  initital import
**
**
**
**************************************************************************/

#include <vector>
#include <iostream>
#include <iomanip>
#include <map>
#include <cmath>
#include <cassert>

#include "ClassificationStatistics.hh"


/*=========================================================================
 *  DESCRIPTION OF FUNCTION:  calcStatistics
 *  ==> see headerfile
 *=======================================================================*/
void svt::ClassificationStatistics::calcStatistics(
    const std::vector<double>& trueLabels,
    const std::vector<double>& predictedLabels,
    std::vector<SingleClassResult>& resultTable)
{
  std::map<int, svt::SingleClassResult> resultMap;
  for( unsigned int i = 0; i < trueLabels.size(); ++i)
  {
    int trueLabel = int( std::floor(trueLabels[i] + 0.5));
    int predictedLabel = int( std::floor(predictedLabels[i] + 0.5));
    resultMap[trueLabel].trueNumber += 1;
    if( predictedLabel == trueLabel)
    {
      resultMap[predictedLabel].truePositives += 1;
    }
    else
    {
      resultMap[predictedLabel].falsePositives += 1;
    }
  }
  resultTable.resize( resultMap.size());
  std::map<int, svt::SingleClassResult>::iterator p = resultMap.begin();
  for( unsigned int i = 0; i < resultMap.size(); ++i)
  {
    p->second.label = p->first;
    resultTable[i] = p->second;
    ++p;
  }
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:  calcStatistics
 *  ==> see headerfile
 *=======================================================================*/
void svt::ClassificationStatistics::prettyPrintStatistics( 
    const std::vector<SingleClassResult>& resultTable, std::ostream& os)
{
  int nFeatureVectors = 0;
  for( std::vector<SingleClassResult>::const_iterator 
           p = resultTable.begin(); p != resultTable.end(); ++p)
  {
    nFeatureVectors += p->trueNumber;
  }

  os << "Classification statistics\n";
  os << "label     truePositives        falsePositives\n";
  os << "----------------------------------------------------\n";
  int sum_truePositives = 0;
  int sum_falsePositives = 0;

  for( std::vector<SingleClassResult>::const_iterator 
           p = resultTable.begin(); p != resultTable.end(); ++p)
  {
    float falseNumber = float(nFeatureVectors - p->trueNumber);
    os << std::setw(5) << p->label 
       << std::setw(7) << p->truePositives << "/" << p->trueNumber 
       << " (" << std::setw(6) << std::setprecision(4)
       << 100.0f * static_cast<float>(p->truePositives) /
        static_cast<float>(p->trueNumber) << "%)"
       << std::setw(7) << p->falsePositives << "/" << falseNumber 
       << " (" << std::setw(7) << std::setprecision(4)
       << 100.0f * static_cast<float>(p->falsePositives) /
        static_cast<float>(falseNumber) << "%)\n";
    
    sum_truePositives  += p->truePositives;
    sum_falsePositives += p->falsePositives;
  }
  
  os << "-----------------------------------------------------\n";
  os << std::setw(5) << "total:"
     << std::setw(7) << sum_truePositives << "/" << nFeatureVectors 
     << " (" << std::setw(6) << std::setprecision(4)
     << 100.0f * static_cast<float>(sum_truePositives) /
      static_cast<float>(nFeatureVectors) << "%)"
     << std::setw(7) << sum_falsePositives << "/" << nFeatureVectors 
     << " (" << std::setw(7) << std::setprecision(4)
     << 100.0f * static_cast<float>(sum_falsePositives) /
      static_cast<float>(nFeatureVectors) << "%)\n";

}


/*=========================================================================
 *  DESCRIPTION OF FUNCTION:  prettyPrintConfusionTable
 *  ==> see headerfile
 *=======================================================================*/
void svt::ClassificationStatistics::prettyPrintConfusionTable( 
    const std::vector<double>& trueLabels,
    const std::vector<double>& predictedLabels,
    std::ostream& os)
{
  assert( trueLabels.size() == predictedLabels.size());
 
  /*-----------------------------------------------------------------------
   *  setup mapping from  lables to index 
   *-----------------------------------------------------------------------*/
  std::map< int, int> labelToIndex;
  for( size_t i = 0; i < trueLabels.size(); ++i)
  {
    int trueLabel = int( std::floor(trueLabels[i] + 0.5));
    int predictedLabel = int( std::floor(predictedLabels[i] + 0.5));
    labelToIndex[trueLabel] = 0;
    labelToIndex[predictedLabel] = 0;
  }
  int index = 0;
  for( std::map< int, int>::iterator p = labelToIndex.begin(); 
       p != labelToIndex.end(); ++p, ++index)
  {
    p->second = index;
  }
  
  /*-----------------------------------------------------------------------
   *  create 2D confusion table and fill it
   *-----------------------------------------------------------------------*/
  size_t nrows = labelToIndex.size();
  size_t ncols = labelToIndex.size();
  std::vector<int> table( nrows * ncols);
  for( size_t i = 0; i < nrows*ncols; ++i)
  {
    table[i] = 0;
  }
  
  for( size_t i = 0; i < trueLabels.size(); ++i)
  {
    int trueLabel = int( std::floor(trueLabels[i] + 0.5));
    int predictedLabel = int( std::floor(predictedLabels[i] + 0.5));
    int row = labelToIndex[trueLabel];
    int col = labelToIndex[predictedLabel];
    table[row * ncols + col] += 1;
  }
  
  /*-----------------------------------------------------------------------
   *  pretty print  table
   *-----------------------------------------------------------------------*/
  os << "Confusion table\n"
      "--------------------\n";
  os << "     |";
  for( std::map< int, int>::iterator p = labelToIndex.begin(); 
       p != labelToIndex.end(); ++p, ++index)
  {
    os << std::setw(5) << p->first;
  }
  os << "         \n";
  os << "-----+------------------------------------------------------------\n";
  
  std::map< int, int>::iterator p2 = labelToIndex.begin();
  for( size_t row = 0; row < nrows; ++row, ++p2)
  {
    os << std::setw(5) << p2->first << "|";
    for( size_t col = 0; col < ncols; ++col)
    {
      if( row == col)
      {
        os << "\033[1m";
      }
      os << std::setw(5) << table[row* ncols + col];
      if( row == col)
      {
        os << "\033[0m";
      }
    }
    os << "        \n";
  }
  
    

}
