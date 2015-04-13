/**************************************************************************
 *
 * Copyright (C) 2004-2015 Olaf Ronneberger, Florian Pigorsch, Jörg Mechnich,
 *                         Thorsten Falk
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


