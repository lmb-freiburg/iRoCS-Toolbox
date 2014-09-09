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
**  Revision 1.1  2004/08/26 08:36:58  ronneber
**  initital import
**
**
**
**************************************************************************/

#include <vector>
#include "CrossValidator.hh"

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:  generateSortedSubsets
 *  ==> see headerfile
 *=======================================================================*/
void svt::generateSortedSubsets( int nFeatureVectors, int nSubsets,
                            std::vector<int>& subsetIndizesForFVs)
{
  float vectorsPerSubset = static_cast<float>(nFeatureVectors) /
      static_cast<float>(nSubsets);
  subsetIndizesForFVs.resize( nFeatureVectors);
  for( int i = 0; i < nFeatureVectors; ++i)
  {
    subsetIndizesForFVs[i] = static_cast<int>(
        static_cast<float>(i) / vectorsPerSubset);
  }
}


/*=========================================================================
 *  DESCRIPTION OF FUNCTION:  generateShuffledSubsets
 *  ==> see headerfile
 *=======================================================================*/
void svt::generateShuffledSubsets( int nFeatureVectors, int nSubsets,
                              std::vector<int>& subsetIndizesForFVs)
{
  generateSortedSubsets( nFeatureVectors, nSubsets, subsetIndizesForFVs);
  std::random_shuffle( subsetIndizesForFVs.begin(), subsetIndizesForFVs.end());
}


